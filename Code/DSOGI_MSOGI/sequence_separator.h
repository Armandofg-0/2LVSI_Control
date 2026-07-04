#ifndef SEQUENCE_SEPARATOR_H
#define SEQUENCE_SEPARATOR_H

#include "../DSOGI_MSOGI/dsogi.h"

/* Symmetric Components in alpha-beta frame based on the fundamental DSOGI
   (msogi.dsogi_h[0]).*/
typedef struct {
    double v_alpha_pos;
    double v_beta_pos;
    double v_alpha_neg;
    double v_beta_neg;
} SeqSep;

void SeqSep_step(SeqSep *s, const DSOGI *fundamental);

#endif /* SEQUENCE_SEPARATOR_H */
