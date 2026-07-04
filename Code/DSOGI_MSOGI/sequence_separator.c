#include "sequence_separator.h"

void SeqSep_step(SeqSep *s, const DSOGI *fund)
{
    double va = fund->v_alpha;
    double qva = fund->qv_alpha;
    double vb = fund->v_beta;
    double qvb = fund->qv_beta;

    /* qv leads v by 90 degrees, hence these signs */
    s->v_alpha_pos = 0.5 * (va + qvb);
    s->v_beta_pos = 0.5 * (vb - qva);

    s->v_alpha_neg = 0.5 * (va - qvb);
    s->v_beta_neg = 0.5 * (vb + qva);
}
