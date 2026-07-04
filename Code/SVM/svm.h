#ifndef __SVPWM_H__
#define __SVPWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

/* Space-vector PWM, duty-cycle output only (no gate edges: those come from
   a native PLECS triangular carrier + comparators). Adapted from Serhii
   Yatsenko's SVPWM reference implementation. */
typedef struct sSVPWM {
    float V_alpha, V_beta;    /* voltage reference [V] */
    float Vdc;                /* measured DC-link voltage [V] (noisy) */
    float Vdc_nom;            /* nominal/regulated DC-link voltage [V] */
    uint8_t use_nominal_vdc;  /* 1 = normalise with Vdc_nom, 0 = use Vdc */

    float d_a, d_b, d_c;      /* duty cycles [0,1] */

    float Ts, fsw;            /* set by the model at Start; SVM() doesn't use them */
} tSVPWM;

void SVM(tSVPWM *ptSVPWM);

#ifdef __cplusplus
}
#endif

#endif /* __SVPWM_H__ */
