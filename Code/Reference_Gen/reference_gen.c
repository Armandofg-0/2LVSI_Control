#include "reference_gen.h"

#define REFGEN_DEN_FLOOR 1.0  /* keeps denominators from blowing up during PLL startup */

void REFERENCE_GEN_init(REFERENCE_GEN *r)
{
    r->i_alpha_ref = 0.0;
    r->i_beta_ref  = 0.0;
    r->i_alpha_dl  = 0.0;
    r->i_beta_dl   = 0.0;
    r->i_alpha_gl  = 0.0;
    r->i_beta_gl   = 0.0;
    r->p_check     = 0.0;
    r->q_check     = 0.0;
    r->den_inst    = 0.0;
    r->den_dc      = 0.0;
}

void REFERENCE_GEN_step(REFERENCE_GEN *r,
                        double v_alpha_pos, double v_beta_pos,
                        double v_alpha_neg, double v_beta_neg,
                        double v_alpha_wb,  double v_beta_wb,
                        double P_ref, double Q_ref,
                        double k, int ipc_wideband)
{
    const double TWO_THIRDS = 2.0 / 3.0;

    /* fundamental grid voltage = positive + negative sequence */
    double u_alpha = v_alpha_pos + v_alpha_neg;
    double u_beta  = v_beta_pos  + v_beta_neg;

    double num_alpha = P_ref * u_alpha + Q_ref * u_beta;
    double num_beta  = P_ref * u_beta  - Q_ref * u_alpha;

    double den_inst = u_alpha * u_alpha + u_beta * u_beta;  /* IPC denom, carries the 2w ripple */
    double den_dc = (v_alpha_pos * v_alpha_pos + v_beta_pos * v_beta_pos)
                  + (v_alpha_neg * v_alpha_neg + v_beta_neg * v_beta_neg);  /* CBC denom, constant */

    if (den_inst < REFGEN_DEN_FLOOR) den_inst = REFGEN_DEN_FLOOR;
    if (den_dc   < REFGEN_DEN_FLOOR) den_dc   = REFGEN_DEN_FLOOR;

    /* CBC (k=0): low-THD fundamental current, constant denominator */
    r->i_alpha_dl = TWO_THIRDS * num_alpha / den_dc;
    r->i_beta_dl  = TWO_THIRDS * num_beta  / den_dc;

    /* IPC (k=1): constant-power current.
       fundamental (ipc_wideband=0): held constant against u, so actual
       power still ripples on a distorted grid.
       wideband (ipc_wideband=1): num and den use the raw Clarke voltage
       v_wb -> P/Q held constant against the real voltage, more distorted
       current as the price. */
    if (ipc_wideband) {
        double den_wb = v_alpha_wb * v_alpha_wb + v_beta_wb * v_beta_wb;
        if (den_wb < REFGEN_DEN_FLOOR) den_wb = REFGEN_DEN_FLOOR;
        double num_wb_alpha = P_ref * v_alpha_wb + Q_ref * v_beta_wb;
        double num_wb_beta  = P_ref * v_beta_wb  - Q_ref * v_alpha_wb;
        r->i_alpha_gl = TWO_THIRDS * num_wb_alpha / den_wb;
        r->i_beta_gl  = TWO_THIRDS * num_wb_beta  / den_wb;
        r->den_inst   = den_wb;
    } else {
        r->i_alpha_gl = TWO_THIRDS * num_alpha / den_inst;
        r->i_beta_gl  = TWO_THIRDS * num_beta  / den_inst;
        r->den_inst   = den_inst;
    }

    /* coordinated blend */
    r->i_alpha_ref = r->i_alpha_dl + k * (r->i_alpha_gl - r->i_alpha_dl);
    r->i_beta_ref  = r->i_beta_dl  + k * (r->i_beta_gl  - r->i_beta_dl);

    /* power check against the fundamental voltage u (open-loop sanity;
       in wideband IPC the meaningful flatness is against the real grid
       voltage, so measure that separately in the model if needed) */
    r->p_check = 1.5 * (u_alpha * r->i_alpha_ref + u_beta * r->i_beta_ref);
    r->q_check = 1.5 * (u_beta  * r->i_alpha_ref - u_alpha * r->i_beta_ref);
    r->den_dc  = den_dc;
}
