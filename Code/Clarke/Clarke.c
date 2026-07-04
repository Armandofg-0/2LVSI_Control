#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Clarke.h"


// Initialization of constants
const double THREE_INV	= 0.33333333333333333333333333333333;		// <- 1/3
const double SQRT_THREE	= 1.7320508075688772935274463415059;		// <- sqrt(3)
const double SQRT_THREE_INV	= 0.57735026918962576450914878050196;   // <- 1/sqrt(3)


/*
 * Performs the Clarke Transformation, which converts three-phase signals (A, B, C)
 * into an orthogonal two-axis system (α, β) and a zero-sequence component. The 
 * transformation preserves signal amplitude (amplitude-invariant).
 *
 * Parameters:
 *  A, B, C     - Instantaneous values of the three-phase voltages or currents.
 *  clarke      - Pointer to the structure where the α, β, and zero-sequence values will be stored.
 */
void Transform_ABC_to_Clarke(FORM_CLARKE *clarke, float A, float B, float C){
    clarke->alpha = THREE_INV * (2.0*A - B - C);            // Projection onto the alpha axis
    clarke->beta = SQRT_THREE_INV * (B - C);                // Projection onto the beta axis
    clarke->zero = THREE_INV *(A + B + C);                  // Projection onto the zero-sequence component
}


/* 
 * Performs the inverse Clarke Transformation, which converts an orthogonal two-axis system 
 * (α, β) and a zero-sequence component into three-phase signals (A, B, C).
 *
 * Parameters:
 *   abc    - Pointer to the structure where the resulting phase values (A, B, C) will be stored.
 *   alpha  - Alpha-axis component from Clarke transform.
 *   beta   - Beta-axis component from Clarke transform.
 *   zero   - Zero-sequence component (common-mode voltage or current).
 */
void Transform_Clarke_to_ABC(FORM_ABC *abc, float alpha, float beta, float zero){
    abc->A = alpha + zero;                                          // Projection onto phase A
    abc->B = - 0.5 * alpha + 0.5 * (SQRT_THREE * beta) + zero;      // Projection onto phase B
    abc->C = - 0.5 * alpha - 0.5 * (SQRT_THREE * beta) + zero;      // Projection onto phase C
}
