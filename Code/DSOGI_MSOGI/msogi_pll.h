#ifndef MSOGI_PLL_H
#define MSOGI_PLL_H

#include "../DSOGI_MSOGI/msogi.h"
#include "../DSOGI_MSOGI/sequence_separator.h"
#include "../Clarke/Clarke.h"
#include "../Park/Park.h"
#include "../PI_Controller/PI.h"
#include "../Low_Pass_Filter/low_pass_filter_1degree.h"

#include "../DSOGI_MSOGI/dsogi.c"
#include "../Clarke/Clarke.c"
#include "../Park/Park.c"
#include "../PI_Controller/PI.c"
#include "../Low_Pass_Filter/low_pass_filter_1degree.c"
#include "../DSOGI_MSOGI/msogi.c"
#include "../DSOGI_MSOGI/sequence_separator.c"

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Grid-sync PLL for unbalanced/distorted grids.
   Chain: Clarke -> MSOGI (CDN bank) -> SeqSep -> Park -> PI -> theta.
   Read outputs straight from the struct: theta_out, omega_out,
   v_alpha_pos/v_beta_pos, v_d/v_q; negative sequence via seq.* */
typedef struct {
    double Ts;
    double k_sogi;

    double omega_nominal;
    double omega;
    double omega_prev;  /* for the trapezoidal integration of theta */
    double theta;
    double theta_out;   /* exported angle, includes delay/convention offset */

    FORM_CLARKE clarke;
    FORM_PARK park;
    MSOGI msogi;
    SeqSep seq;

    double v_alpha_pos;
    double v_beta_pos;
    double v_zero;
    double v_d;
    double v_q;

    PI_FORM pi;
    LP_FILTER_1DEGREE lpf_omega;

    float vq_filt;      /* normalised q error, IIR filtered (PI input) */
    double omega_out;
    int startup_samples;

    int locked;
    int lock_count;     /* consecutive samples meeting the lock criterion */

    /* startup-only phase trim: speeds up the first lock, then latches
       off for good (left running it makes omega drift, see .c) */
    int trim_active;
} MSOGI_PLL;

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
                    double *harmonics);

void MSOGI_PLL_step(MSOGI_PLL *pll, double Va, double Vb, double Vc);

#endif /* MSOGI_PLL_H */
