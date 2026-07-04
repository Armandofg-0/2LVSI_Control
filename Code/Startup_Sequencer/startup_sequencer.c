#include "startup_sequencer.h"

void STARTUP_SEQ_init(STARTUP_SEQ *s, double Ts, double T_min_sync, double T_ramp)
{
    s->Ts = Ts;
    s->T_min_sync = T_min_sync;
    s->T_ramp = (T_ramp > 0.0) ? T_ramp : Ts;  /* guard /0 */

    s->t = 0.0;
    s->t_enable = 0.0;
    s->state = SEQ_SYNC;

    s->gate_enable = 0;
    s->p_scale = 0.0;
}

void STARTUP_SEQ_step(STARTUP_SEQ *s, int pll_locked)
{
    s->t += s->Ts;

    /* transitions evaluated before outputs so the new state applies this
       same step, no one-tick lag */
    if (s->state == SEQ_SYNC) {
        if (pll_locked && s->t >= s->T_min_sync) {
            s->state = SEQ_RAMP;
            s->t_enable = s->t;
        }
    } else if (s->state == SEQ_RAMP) {
        if ((s->t - s->t_enable) >= s->T_ramp)
            s->state = SEQ_RUN;
    }

    switch (s->state) {
        case SEQ_SYNC:
            s->gate_enable = 0;
            s->p_scale = 0.0;
            break;

        case SEQ_RAMP:
            s->gate_enable = 1;
            s->p_scale = (s->t - s->t_enable) / s->T_ramp;
            if (s->p_scale > 1.0) s->p_scale = 1.0;
            break;

        case SEQ_RUN:
        default:
            s->gate_enable = 1;
            s->p_scale = 1.0;
            break;
    }
}
