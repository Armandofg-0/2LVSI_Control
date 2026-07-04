#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Park.h"

/*
 * Performs the Park Transformation, which converts alpha-beta-zero into a rotating d-q-zero
 * components
 *
 * Parameters:
 *  park    - Pointer to the structure where the α, β, and zero-sequence values will be stored.
 *  alpha   - Alpha-axis component from Clarke transform.
 *  beta    - Beta-axis component from Clarke transform.
 *  zero    - Zero-sequence component (common-mode voltage or current).
 *  theta   - Electrical angle (in radians) of the rotating reference frame.
 */
void Transform_Clarke_to_Park(FORM_PARK *park, float alpha, float beta, float zero, float theta){
    park->d =  (alpha * cos(theta)) + (beta * sin(theta));	// Direct-axis projection
	park->q = (-alpha * sin(theta)) + (beta * cos(theta));	// Quadrature-axis projection
	park->zero = zero;										// Zero-sequence remains unchanged
}

/*
 * Performs the inverse Park Transformation, which converts d-q-zero 
 * into alpha-beta and a zero-sequence component.
 *
 * Parameters:
 * 	clarke  - Pointer to the structure where the α, β, and zero-sequence values will be stored.
 *  d       - Direct-axis component in the rotating frame.
 *  q       - Quadrature-axis component in the rotating frame.
 *  zero    - Zero-sequence component (remains unchanged).
 *  theta   - Electrical angle (in radians) of the rotating reference frame.
 */
void Transform_Park_to_Clarke(FORM_CLARKE *clarke, float d, float q, float zero, float theta){
	clarke->alpha = (d * cos(theta)) - (q * sin(theta));	// Project onto alpha axis
	clarke->beta  = (d * sin(theta)) + (q * cos(theta));	// Project onto beta axis
	clarke->zero = zero;									// Zero-sequence remains unchanged
}