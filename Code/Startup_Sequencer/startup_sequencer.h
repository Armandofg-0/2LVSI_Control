#ifndef STARTUP_SEQUENCER_H
#define STARTUP_SEQUENCER_H

/* Grid-tie connection state machine: sync the PLL with the gates blocked,
   then once locked (and after a minimum sync time) unblock the gates and
   ramp the current reference up smoothly. One-shot, no re-sync path.

   SYNC (gates blocked, p_scale=0) -> RAMP (gates on, p_scale 0->1 over
   T_ramp) -> RUN (p_scale=1). SYNC->RAMP needs pll_locked AND t>=T_min_sync.

   During SYNC the gates are blocked so i_meas=i_ref=0, so the PCI's
   resonators never wind up -- no explicit reset needed there. */

enum { SEQ_SYNC = 0, SEQ_RAMP = 1, SEQ_RUN = 2 };

typedef struct {
    double Ts;
    double T_min_sync;  /* min time held in SYNC before connecting [s] */
    double T_ramp;      /* current soft-start ramp duration [s] */

    double t;
    double t_enable;    /* time of the SYNC->RAMP transition [s] */
    int    state;

    int    gate_enable; /* AND with the 6 gate signals */
    double p_scale;     /* 0..1, multiplies i_alpha_ref/i_beta_ref */
} STARTUP_SEQ;

void STARTUP_SEQ_init(STARTUP_SEQ *s, double Ts, double T_min_sync, double T_ramp);

/* pll_locked: lock flag from MSOGI_PLL (1 = synchronised) */
void STARTUP_SEQ_step(STARTUP_SEQ *s, int pll_locked);

#endif /* STARTUP_SEQUENCER_H */
