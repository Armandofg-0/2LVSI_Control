#ifndef CLARKE_H
#define CLARKE_H

/*
 * Structure representing signals in Clarke (α-β-0) form.
 *
 * Stores voltage or current values transformed into the 
 * orthogonal Clarke reference frame (alpha, beta, zero-sequence).
*/
typedef struct {
    double alpha;
    double beta;
    double zero;
} FORM_CLARKE;


/*
 * Structure representing signals in three-phase (A-B-C) form.
 *
 * Stores instantaneous voltage or current values for each phase.
*/
typedef struct {
    double A;
    double B;
    double C;
} FORM_ABC;


/*
 * Performs the Clarke Transformation.
 *
 * Converts three-phase signals (A, B, C) into α-β-zero components.
*/
void Transform_ABC_to_Clarke(FORM_CLARKE *clarke, float A, float B, float C);

/*
 * Performs the inverse Clarke Transformation.
 *
 * Converts α-β-zero components back into three-phase signals (A, B, C).
*/
void Transform_Clarke_to_ABC(FORM_ABC *abc, float alpha, float beta, float zero);

#endif