#include "pci.h"

void PCI_init(PCI_CTRL *c, double Ts, double Kp, double v_limit)
{
    c->Ts = Ts;
    c->Kp = Kp;
    c->v_limit = v_limit;
    c->comp_delay = 0.0;
    c->leak = 1.0;
    c->num_terms = 0;
    c->v_alpha = 0.0;
    c->v_beta = 0.0;

    for (int i = 0; i < PCI_MAX_TERMS; ++i) {
        c->term[i].n = 0.0;
        c->term[i].Ki = 0.0;
        c->term[i].leak = 1.0;
        c->term[i].s_alpha = 0.0;
        c->term[i].s_beta = 0.0;
    }
}

void PCI_add_term(PCI_CTRL *c, double n, double Ki)
{
    if (c->num_terms >= PCI_MAX_TERMS) return;
    c->term[c->num_terms].n = n;
    c->term[c->num_terms].Ki = Ki;
    c->term[c->num_terms].leak = c->leak;
    c->term[c->num_terms].s_alpha = 0.0;
    c->term[c->num_terms].s_beta = 0.0;
    c->num_terms++;
}

void PCI_set_delay_comp(PCI_CTRL *c, double n_samples)
{
    c->comp_delay = n_samples;
}

void PCI_set_leak(PCI_CTRL *c, double rho)
{
    if (rho > 1.0) rho = 1.0;
    if (rho < 0.0) rho = 0.0;
    c->leak = rho;
    for (int i = 0; i < c->num_terms; ++i)
        c->term[i].leak = rho;
}

void PCI_reset(PCI_CTRL *c)
{
    for (int i = 0; i < c->num_terms; ++i) {
        c->term[i].s_alpha = 0.0;
        c->term[i].s_beta = 0.0;
    }
    c->v_alpha = 0.0;
    c->v_beta = 0.0;
}

static double clampd(double x, double lim)
{
    if (x > lim) return lim;
    if (x < -lim) return -lim;
    return x;
}

void PCI_step(PCI_CTRL *c,
              double i_ref_alpha, double i_ref_beta,
              double i_meas_alpha, double i_meas_beta,
              double omega,
              double ff_alpha, double ff_beta)
{
    double e_alpha = i_ref_alpha - i_meas_alpha;
    double e_beta = i_ref_beta - i_meas_beta;

    double sum_alpha = 0.0;
    double sum_beta = 0.0;

    for (int i = 0; i < c->num_terms; ++i) {
        COMPLEX_INTEGRATOR *t = &c->term[i];

        /* loop-delay compensation: pre-rotate the error so this resonator
           leads by the transport delay (comp_delay=0: no change) */
        double ec_alpha = e_alpha;
        double ec_beta = e_beta;
        if (c->comp_delay != 0.0) {
            double phic = t->n * omega * c->Ts * c->comp_delay;
            double cphi = cos(phic);
            double sphi = sin(phic);
            ec_alpha = cphi * e_alpha - sphi * e_beta;
            ec_beta = sphi * e_alpha + cphi * e_beta;
        }

        t->s_alpha += t->Ki * c->Ts * ec_alpha;
        t->s_beta += t->Ki * c->Ts * ec_beta;

        /* rotate the accumulator by phi = n*omega*Ts -> resonance at n*omega */
        double phi = t->n * omega * c->Ts;
        double cs = cos(phi);
        double sn = sin(phi);
        double ra = cs * t->s_alpha - sn * t->s_beta;
        double rb = sn * t->s_alpha + cs * t->s_beta;

        /* leak pulls the pole to radius t->leak (<=1) inside the unit circle */
        t->s_alpha = t->leak * ra;
        t->s_beta = t->leak * rb;

        /* anti-windup */
        t->s_alpha = clampd(t->s_alpha, c->v_limit);
        t->s_beta = clampd(t->s_beta, c->v_limit);

        sum_alpha += t->s_alpha;
        sum_beta += t->s_beta;
    }

    double v_alpha = c->Kp * e_alpha + sum_alpha + ff_alpha;
    double v_beta = c->Kp * e_beta + sum_beta + ff_beta;

    c->v_alpha = clampd(v_alpha, c->v_limit);
    c->v_beta = clampd(v_beta, c->v_limit);
}
