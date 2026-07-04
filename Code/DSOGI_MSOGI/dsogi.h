#ifndef DSOGI_H
#define DSOGI_H

/* 2 independent SOGIs in quadrature (for alpha and beta axis) */
typedef struct {
    double Ts;
    double E_max;    /* saturation of the input error */
    int step_count;  /* integrator startup: 0=Euler, 1=AB2, >=2 AB3 */

    double v_alpha;
    double qv_alpha;
    double v_beta;
    double qv_beta;

    /* historical derivatives for AB3, a buffer per state */
    double dv_alpha_hist[3];
    double dqv_alpha_hist[3];
    double dv_beta_hist[3];
    double dqv_beta_hist[3];
} DSOGI;

void DSOGI_init(DSOGI *d, double Ts, double E_max);

/* w: resonance frequency, k: damping gain */
void DSOGI_step(DSOGI *d, double v_alpha_in, double v_beta_in, double w, double k);

double DSOGI_get_v_alpha(DSOGI *d);
double DSOGI_get_v_beta(DSOGI *d);
double DSOGI_get_qv_alpha(DSOGI *d);
double DSOGI_get_qv_beta(DSOGI *d);

#endif /* DSOGI_H */
