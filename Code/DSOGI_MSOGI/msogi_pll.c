#include "msogi_pll.h"
#include "../Clarke/Clarke.h"
#include "../Park/Park.h"
#include "../PI_Controller/PI.h"
#include "../Low_Pass_Filter/low_pass_filter_1degree.h"

/* lock criterion: all three must hold for N_LOCK consecutive samples */
#define AMP_LOCK_MIN 50.0           /* min +seq amplitude to be "live" [V] */
#define VQ_LOCK_THR 0.05            /* normalised q error */
#define W_LOCK_BAND (2.0*M_PI*1.0)  /* +/-1 Hz around nominal [rad/s] */
#define N_LOCK 200                  /* 1 grid cycle at Ts=100us */

void MSOGI_PLL_init(MSOGI_PLL *pll,
                    double Ts,
                    double f_nominal,
                    double k_sogi,
                    double E_max,
                    float Kp,
                    float Ki,
                    float pi_limit,
                    float lpf_omega_fc,
                    int num_harmonics,
                    double *harmonics)
{
    pll->Ts = Ts;
    pll->k_sogi = k_sogi;
    pll->omega_nominal = 2.0 * M_PI * f_nominal;
    pll->omega = pll->omega_nominal;
    pll->omega_prev = pll->omega_nominal;
    pll->theta = 0.0;
    pll->theta_out = 0.0;
    pll->clarke.alpha = 0.0;
    pll->clarke.beta = 0.0;

    MSOGI_init(&pll->msogi, Ts, k_sogi, E_max, num_harmonics, harmonics);

    pll->v_alpha_pos = 0.0;
    pll->v_beta_pos = 0.0;
    pll->v_zero = 0.0;
    pll->v_d = 0.0;
    pll->v_q = 0.0;
    pll->vq_filt = 0.0f;
    pll->omega_out = pll->omega_nominal;
    pll->startup_samples = 0;
    pll->locked = 0;
    pll->lock_count = 0;
    pll->trim_active = 1;

    INIT_PI(&pll->pi, Kp, Ki, (float)Ts, pi_limit);
    INIT_LP_FILTER_1D(&pll->lpf_omega, lpf_omega_fc, (float)Ts);
}

void MSOGI_PLL_step(MSOGI_PLL *pll, double Va, double Vb, double Vc)
{
    /* soft start: hold nominal omega for 100 samples, then blend in the
       PI estimate over the next 100 (counts hardcoded for Ts=100us) */
    pll->startup_samples++;
    double ramp;
    if (pll->startup_samples < 100) ramp = 0.0;
    else if (pll->startup_samples < 200) ramp = (pll->startup_samples - 100) / 100.0;
    else ramp = 1.0;

    double omega_for_sogi = (1.0 - ramp) * pll->omega_nominal + ramp * pll->omega;

    /* Clarke */
    Transform_ABC_to_Clarke(&pll->clarke, (float)Va, (float)Vb, (float)Vc);
    double v_alpha = (double)pll->clarke.alpha;
    double v_beta = (double)pll->clarke.beta;
    pll->v_zero = (double)pll->clarke.zero;

    /* MSOGI bank + sequence separation on the fundamental */
    MSOGI_step(&pll->msogi, v_alpha, v_beta, omega_for_sogi);
    SeqSep_step(&pll->seq, &pll->msogi.dsogi_h[0]);
    pll->v_alpha_pos = pll->seq.v_alpha_pos;
    pll->v_beta_pos = pll->seq.v_beta_pos;

    /* Park; vq normalised by the +seq amplitude (10 V floor so the
       division doesn't blow up at startup) */
    double amp = sqrt(pll->v_alpha_pos * pll->v_alpha_pos +
                      pll->v_beta_pos * pll->v_beta_pos);
    if (amp < 10.0) amp = 10.0;

    Transform_Clarke_to_Park(&pll->park,
                             (float)pll->v_alpha_pos,
                             (float)pll->v_beta_pos,
                             0.0f,
                             (float)pll->theta);

    pll->vq_filt = 0.98f * pll->vq_filt + 0.02f * (float)(pll->park.q / amp);
    pll->v_d = pll->park.d;
    pll->v_q = pll->park.q;

    /* PI -> omega */
    CALCULATE_PI(&pll->pi, pll->vq_filt);
    pll->omega = pll->omega_nominal + (double)pll->pi.out;

    /* theta, trapezoidal integration */
    pll->theta += 0.5 * pll->Ts * (pll->omega + pll->omega_prev);
    pll->omega_prev = pll->omega;

    /* startup-only phase trim: being a direct angle correction it locks
       much faster than the PI alone, but left running it keeps vq_filt
       from ever settling to zero and the PI integral drifts omega until
       it rails at pi_limit so it latches off at first lock (not recommended
       for a real implementation) */
    if (pll->trim_active) {
        double theta_inst = atan2(pll->v_beta_pos, pll->v_alpha_pos);
        double theta_err = theta_inst - pll->theta;
        if (theta_err > M_PI) theta_err -= 2.0 * M_PI;
        else if (theta_err < -M_PI) theta_err += 2.0 * M_PI;
        pll->theta += 0.01 * theta_err;
    }

    if (pll->theta >= 2.0 * M_PI) pll->theta -= 2.0 * M_PI;
    else if (pll->theta < 0.0) pll->theta += 2.0 * M_PI;

    /* exported angle: one sample ahead plus a convention offset
       measured empirically at 2.273 ms */
    #define T_CONV_OFFSET 0.002273
    pll->theta_out = pll->theta + pll->omega * (pll->Ts + T_CONV_OFFSET);
    if (pll->theta_out >= 2.0 * M_PI) pll->theta_out -= 2.0 * M_PI;
    else if (pll->theta_out < 0.0) pll->theta_out += 2.0 * M_PI;

    /* smoothed omega, monitoring/feedforward only (theta integrates
       the raw one) */
    CALCULATE_LP_FILTER_1D(&pll->lpf_omega, (float)pll->omega);
    pll->omega_out = (double)pll->lpf_omega.Y;

    /* lock detection */
    double amp_pos = sqrt(pll->v_alpha_pos * pll->v_alpha_pos +
                          pll->v_beta_pos * pll->v_beta_pos);
    int crit = (amp_pos > AMP_LOCK_MIN) &&
               (fabs((double)pll->vq_filt) < VQ_LOCK_THR) &&
               (fabs(pll->omega - pll->omega_nominal) < W_LOCK_BAND);
    if (crit) { if (pll->lock_count < N_LOCK) pll->lock_count++; }
    else pll->lock_count = 0;
    pll->locked = (pll->lock_count >= N_LOCK) ? 1 : 0;

    /* one-way latch, the trim never comes back on */
    if (pll->locked) pll->trim_active = 0;
}
