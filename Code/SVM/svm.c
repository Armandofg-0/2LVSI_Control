#include "svm.h"
#include <math.h>

const double Pi       = 3.14159265358979323846264338327950;
const double PI_3     = 1.04719755119659774615421446109317;
const double INV_PI_3 = 0.95492965855137201461330258023509;
const double INV_SQRT_THREE = 0.57735026918962576450914878050196;

/* permutation for 7-segment symmetric SVPWM, indexed by sector */
const uint8_t TI_Matrix[6][3] =
{
    { 3, 2, 0 },  // Sector 0
    { 1, 3, 0 },  // Sector 1
    { 0, 3, 2 },  // Sector 2
    { 0, 1, 3 },  // Sector 3
    { 2, 0, 3 },  // Sector 4
    { 3, 0, 1 }   // Sector 5
};

/* caps |V| below the linear ceiling so no duty reaches exactly 0 or 1
   (keeps a minimum on-time, avoids zero-width pulses) */
#define SVM_MOD_MARGIN 0.02f

void SVM(tSVPWM* ptSVPWM)
{
    float V = sqrtf(ptSVPWM->V_alpha * ptSVPWM->V_alpha +
                    ptSVPWM->V_beta  * ptSVPWM->V_beta);

    /* angle of the reference vector itself, not the PLL: the PCI/PMCI
       output carries negative-sequence and harmonic phase that must be
       rendered as-is */
    float Theta = atan2f(ptSVPWM->V_beta, ptSVPWM->V_alpha);
    while (Theta < 0.0f)             Theta += (float)(2.0 * Pi);
    while (Theta >= (float)(2.0 * Pi)) Theta -= (float)(2.0 * Pi);

    /* Vdc_nom avoids feeding bus-voltage ripple/noise into the duty cycle
       when the link is stiff and regulated; use_nominal_vdc switches back
       to the measured value otherwise */
    float Vdc_eff = ptSVPWM->use_nominal_vdc ? ptSVPWM->Vdc_nom : ptSVPWM->Vdc;
    float V_max = Vdc_eff * INV_SQRT_THREE;
    float V_lin = V_max * (1.0f - SVM_MOD_MARGIN);
    if (V > V_lin) V = V_lin;
    float V_scaled = V / V_max;

    uint8_t sector = (uint8_t)(Theta * INV_PI_3) % 6;
    float alpha = Theta - sector * PI_3;

    float T1 = V_scaled * sinf(PI_3 - alpha);
    float T2 = V_scaled * sinf(alpha);

    float T_intervals[4];
    T_intervals[0] = (1.0f - T1 - T2) * 0.5f;
    T_intervals[1] = T1 + T_intervals[0];
    T_intervals[2] = T2 + T_intervals[0];
    T_intervals[3] = T1 + T2 + T_intervals[0];

    ptSVPWM->d_a = T_intervals[TI_Matrix[sector][0]];
    ptSVPWM->d_b = T_intervals[TI_Matrix[sector][1]];
    ptSVPWM->d_c = T_intervals[TI_Matrix[sector][2]];

    if (ptSVPWM->d_a > 1.0f) ptSVPWM->d_a = 1.0f;
    if (ptSVPWM->d_a < 0.0f) ptSVPWM->d_a = 0.0f;
    if (ptSVPWM->d_b > 1.0f) ptSVPWM->d_b = 1.0f;
    if (ptSVPWM->d_b < 0.0f) ptSVPWM->d_b = 0.0f;
    if (ptSVPWM->d_c > 1.0f) ptSVPWM->d_c = 1.0f;
    if (ptSVPWM->d_c < 0.0f) ptSVPWM->d_c = 0.0f;
}
