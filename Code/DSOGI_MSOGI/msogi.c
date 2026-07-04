#include "msogi.h"

void MSOGI_init(MSOGI *m, double Ts, double k_sogi, double E_max,
                int num_harmonics, double *harmonics)
{
    m->Ts = Ts;
    m->k_sogi = k_sogi;

    if (num_harmonics > MAX_HARMONICS)
        num_harmonics = MAX_HARMONICS;
    m->num_harmonics = num_harmonics;

    for (int i = 0; i < num_harmonics; i++) {
        m->harmonics[i] = harmonics[i];
        DSOGI_init(&m->dsogi_h[i], Ts, E_max);
    }
}

void MSOGI_step(MSOGI *m, double alpha, double beta, double omega)
{
    /* It uses the frozen values of the outputs from the previous step, so that the
       decoupling does not depend on the order in which the bank is updated */
    double v_a_old[MAX_HARMONICS];
    double v_b_old[MAX_HARMONICS];
    double sum_alpha = 0.0, sum_beta = 0.0;

    for (int i = 0; i < m->num_harmonics; i++) {
        v_a_old[i] = m->dsogi_h[i].v_alpha;
        v_b_old[i] = m->dsogi_h[i].v_beta;
        sum_alpha += v_a_old[i];
        sum_beta += v_b_old[i];
    }

    /* CDN: each DSOGI sees the input minus what is estimated by the others.
       k is scaled by 1/h to maintain the absolute bandwidth. */
    for (int i = 0; i < m->num_harmonics; i++) {
        double in_alpha = alpha - (sum_alpha - v_a_old[i]);
        double in_beta = beta - (sum_beta - v_b_old[i]);
        double h = m->harmonics[i];

        DSOGI_step(&m->dsogi_h[i], in_alpha, in_beta, h * omega, m->k_sogi / h);
    }
}
