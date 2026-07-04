#ifndef REFERENCE_GEN_H
#define REFERENCE_GEN_H

#include <math.h>

/* Coordinated current-reference generator (alpha-beta). Blends two
   classic strategies with the same numerator (P*u_a+Q*u_b, P*u_b-Q*u_a)
   and different denominators, weighted by k in [0,1]:

     CBC (k=0): den = (U+)^2 + (U-)^2, DC -> low-THD current, power ripples
     IPC (k=1): den = u_a^2 + u_b^2,   instantaneous -> constant power,
                distorted current

     i*_a = i*_a,dl + k*(i*_a,gl - i*_a,dl)

   u = v_pos + v_neg is the fundamental grid voltage, reconstructed from
   the MSOGI_PLL sequence outputs. The CBC denominator is exact (no
   filter, no lag): (U+)^2 = |v_pos|^2, (U-)^2 = |v_neg|^2.

   ipc_wideband selects what IPC (k=1) holds constant against:
     0 = fundamental voltage u -> actual power still ripples on a
         distorted grid
     1 = wideband, raw Clarke voltage v_wb (harmonics included) -> P/Q
         held constant against the real voltage (p-q theory), at the
         cost of a more distorted current and an effectively infinite
         reference spectrum (needs the resonator bank to keep up)

   Stateless / purely algebraic: safe to run entirely in the Output cell.
   Clarke is amplitude-invariant, so the 2/3 power scaling below is exact. */

typedef struct {
    double i_alpha_ref;
    double i_beta_ref;

    /* pure strategies, for debug/validation */
    double i_alpha_dl;   /* CBC, k=0 */
    double i_beta_dl;
    double i_alpha_gl;   /* IPC, k=1 */
    double i_beta_gl;

    double p_check;      /* 1.5*(u_a*i_a + u_b*i_b) */
    double q_check;      /* 1.5*(u_b*i_a - u_a*i_b) */

    double den_inst;     /* u_a^2 + u_b^2   (or wideband equivalent) */
    double den_dc;       /* (U+)^2 + (U-)^2 */
} REFERENCE_GEN;

void REFERENCE_GEN_init(REFERENCE_GEN *r);

/* v_*_pos / v_*_neg: fundamental sequence alpha-beta from MSOGI_PLL
   v_*_wb           : raw wideband Clarke alpha-beta (MSOGI_PLL ports 6/7)
   P_ref [W], Q_ref [VAr], k in [0,1], ipc_wideband: 0=fundamental, 1=wideband */
void REFERENCE_GEN_step(REFERENCE_GEN *r,
                        double v_alpha_pos, double v_beta_pos,
                        double v_alpha_neg, double v_beta_neg,
                        double v_alpha_wb,  double v_beta_wb,
                        double P_ref, double Q_ref,
                        double k, int ipc_wideband);

#endif /* REFERENCE_GEN_H */
