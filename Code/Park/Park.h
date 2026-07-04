#ifndef PARK_H
#define PARK_H
#include "../Clarke/Clarke.h"
/*
 * Structure representing signals in Park (d-q-0) form.
 *
 * Stores voltage or current values transformed into the 
 * orthogonal Park reference frame (d, q, zero-sequence).
*/
typedef struct {
    double d;
    double q;
    double zero;
} FORM_PARK;

/*
 * Performs the Park Transformation.
 *
 * Converts three-phase signals α-β-zero into d-q-zero components.
*/
void Transform_Clarke_to_Park(FORM_PARK *park, float alpha, float beta, float zero, float theta);

/*
 * Performs the inverse Park Transformation.
 *
 * Converts three-phase signals d-q-zero into α-β-zero components.
*/
void Transform_Park_to_Clarke(FORM_CLARKE *clarke, float d, float q, float zero, float theta);

#endif