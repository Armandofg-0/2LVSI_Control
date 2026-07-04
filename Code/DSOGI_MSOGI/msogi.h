#ifndef MSOGI_H
#define MSOGI_H

#include "../DSOGI_MSOGI/dsogi.h"

#define MAX_HARMONICS 8

/* DSOGIS bank, one per harmonic for the CDN network.*/
typedef struct {
    double Ts;
    double k_sogi;
    int num_harmonics;
    double harmonics[MAX_HARMONICS];  /* ej {1.0, 5.0, 7.0} */
    DSOGI dsogi_h[MAX_HARMONICS];
} MSOGI;

/* E_max: saturation error for each DSOGI */
void MSOGI_init(MSOGI *m, double Ts, double k_sogi, double E_max,
                int num_harmonics, double *harmonics);

void MSOGI_step(MSOGI *m, double alpha, double beta, double omega);

#endif /* MSOGI_H */
