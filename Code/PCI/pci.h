#ifndef PCI_H
#define PCI_H

#include <math.h>

/* Proportional + (multiple) complex integrators, stationary alpha-beta frame.
   v = Kp*e + sum_i CI_i(e) + ff, each CI_i resonant at n_i*omega (n signed:
   +1 pos. seq., -1 neg. seq., +5, -5... for harmonics).*/

#define PCI_MAX_TERMS 28  /* headroom for +-sequence harmonic pairs up to ~25th */

typedef struct {
    double n;          /* harmonic order, signed */
    double Ki;
    double leak;       /* pole radius <=1, 1 = ideal */
    double s_alpha;
    double s_beta;
} COMPLEX_INTEGRATOR;

typedef struct {
    double Ts;
    double Kp;
    double v_limit;     /* per-axis saturation */
    double comp_delay;  /* loop-delay compensation [samples], 0 = off */
    double leak;        /* default pole radius for new terms */
    int    num_terms;
    COMPLEX_INTEGRATOR term[PCI_MAX_TERMS];

    double v_alpha;
    double v_beta;
} PCI_CTRL;

void PCI_init(PCI_CTRL *c, double Ts, double Kp, double v_limit);
void PCI_add_term(PCI_CTRL *c, double n, double Ki);
void PCI_reset(PCI_CTRL *c);

/* rho in (0,1]: pole radius, 1 = ideal/marginal. <1 damps the resonator so
   it doesn't ramp unbounded when excited off its own harmonic (gain becomes
   ~1/(1-rho) instead of infinite) */
void PCI_set_leak(PCI_CTRL *c, double rho);

/* pre-rotates each resonator's error by n*omega*(n_samples*Ts) to compensate
   the computation + PWM transport delay; 0 disables it */
void PCI_set_delay_comp(PCI_CTRL *c, double n_samples);

void PCI_step(PCI_CTRL *c,
              double i_ref_alpha, double i_ref_beta,
              double i_meas_alpha, double i_meas_beta,
              double omega,
              double ff_alpha, double ff_beta);

#endif /* PCI_H */
