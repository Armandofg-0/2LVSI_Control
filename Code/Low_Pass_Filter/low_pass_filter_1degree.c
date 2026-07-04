#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LOW_PASS_FILTER_1DEGREE.h"

#define PI 3.14159265358979323846

/*
 * Initializes a first-order low-pass filter using the Bilinear Transform method.
 *
 * The function receives as input cutoff frequency and discretization time and computes the necessary 
 * coefficients using the Bilinear Transform method.
 *
 * Parameters:
 * filter               - Pointer to the LP_FILTER_1DEGREE structure.
 * CUTOFF_FREQUENCY     - Cutoff frequency in Hz.
 * SAMPLING_PERIOD      - Discretization sampling period in seconds.
 */
void INIT_LP_FILTER_1D(LP_FILTER_1DEGREE* filter, float CUTOFF_FREQUENCY, float SAMPLING_PERIOD) {

    // Reset internal filter states to zero
    filter->X_1 = 0.0;
    filter->Y_1 = 0.0;
    filter->X = 0.0;
    filter->Y = 0.0;

    // Calculates the angular frequency (rad/s) from the cutoff frequency in Hz
    float OMEGA = 2.0 * PI * CUTOFF_FREQUENCY;
    
    // Compute filter coefficients using the Bilinear Transform method
    filter->A1 = (SAMPLING_PERIOD*OMEGA - 2.0) / ((SAMPLING_PERIOD*OMEGA + 2.0));
    filter->B0 = (SAMPLING_PERIOD * OMEGA)/ ((SAMPLING_PERIOD*OMEGA + 2.0));
}


/*
 * Initializes a first-order low-pass filter using predefined coefficients.
 *
 * This function resets the internal state of the filter and assigns the provided
 * coefficients directly, without performing any calculation.
 *
 * Parameters:
 * filter   - Pointer to the LP_FILTER_1DEGREE structure.
 * A1       - Coefficient applied to the previous output sample (feedback term).
 * B0       - Coefficient applied to the current and previous input samples (feedforward term).
 */
void INIT_LP_FILTER_1D_CONSTANT(LP_FILTER_1DEGREE* filter, float A1, float B0) {

    // Reset internal filter states to zero
    filter->X_1 = 0;
    filter->Y_1 = 0;
    filter->X = 0;
    filter->Y = 0;

    // Set filter coefficients directly
    filter->A1 = A1;
    filter->B0 = B0;
}


/*
 * Processes a single input sample through a first-order low-pass filter.
 *
 * This function applies the difference equation to compute the filtered output
 * based on the current and previous input/output samples.
 *
 * filter   - Pointer to the LP_FILTER_1DEGREE structure.
 * INPUT    - Current input sample to be filtered.
 */
void CALCULATE_LP_FILTER_1D(LP_FILTER_1DEGREE* filter, float INPUT){

    // Assign the new input
    filter->X = INPUT;

    // Compute the filter output using the difference equation
    filter->Y = filter->B0 * filter->X + filter->B0 * filter->X_1 - filter->A1 * filter->Y_1;

    // Update previous samples for the next iteration
    filter->Y_1 = filter->Y;
    filter->X_1 = filter->X;
}
