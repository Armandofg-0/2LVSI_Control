#include "dsogi.h"
#include <math.h>

/* Multistep: Euler the first step, AB2 the
   second and AB3 from there on */
static double integrador_3(double *hist, double input, double Ts, int step_count)
{
    hist[2] = hist[1];
    hist[1] = hist[0];
    hist[0] = input;

    if (step_count == 0)
        return Ts * hist[0];
    else if (step_count == 1)
        return (Ts / 2.0) * (3.0 * hist[0] - hist[1]);
    else
        return (Ts / 12.0) * (23.0 * hist[0] - 16.0 * hist[1] + 5.0 * hist[2]);
}

void DSOGI_init(DSOGI *d, double Ts, double E_max)
{
    d->Ts = Ts;
    d->E_max = E_max;
    d->step_count = 0;

    d->v_alpha = 0.0;
    d->qv_alpha = 0.0;
    d->v_beta = 0.0;
    d->qv_beta = 0.0;

    for (int i = 0; i < 3; i++) {
        d->dv_alpha_hist[i] = 0.0;
        d->dqv_alpha_hist[i] = 0.0;
        d->dv_beta_hist[i] = 0.0;
        d->dqv_beta_hist[i] = 0.0;
    }
}

void DSOGI_step(DSOGI *d, double v_alpha_in, double v_beta_in, double w, double k)
{
    /* saturated tracking error (avoids windup during startup/jumps) */
    double e_alpha = v_alpha_in - d->v_alpha;
    double e_beta = v_beta_in - d->v_beta;

    if (e_alpha > d->E_max) e_alpha = d->E_max;
    if (e_alpha < -d->E_max) e_alpha = -d->E_max;
    if (e_beta > d->E_max) e_beta = d->E_max;
    if (e_beta < -d->E_max) e_beta = -d->E_max;

    /* derivatives with the states of the previous step */
    double dv_alpha = k * w * e_alpha + w * d->qv_alpha;
    double dqv_alpha = -w * d->v_alpha;
    double dv_beta = k * w * e_beta + w * d->qv_beta;
    double dqv_beta = -w * d->v_beta;

    d->v_alpha += integrador_3(d->dv_alpha_hist, dv_alpha, d->Ts, d->step_count);
    d->qv_alpha += integrador_3(d->dqv_alpha_hist, dqv_alpha, d->Ts, d->step_count);
    d->v_beta += integrador_3(d->dv_beta_hist, dv_beta, d->Ts, d->step_count);
    d->qv_beta += integrador_3(d->dqv_beta_hist, dqv_beta, d->Ts, d->step_count);

    if (d->step_count < 2)
        d->step_count++;
}

double DSOGI_get_v_alpha(DSOGI *d) { return d->v_alpha; }
double DSOGI_get_v_beta(DSOGI *d) { return d->v_beta; }
double DSOGI_get_qv_alpha(DSOGI *d) { return d->qv_alpha; }
double DSOGI_get_qv_beta(DSOGI *d) { return d->qv_beta; }
