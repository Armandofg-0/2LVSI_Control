#ifndef LOW_PASS_FILTER_1DEGREE_H
#define LOW_PASS_FILTER_1DEGREE_H

/*
 * Structure representing a first-order low-pass filter Bilinear Transform.
 *
 * Stores the internal state of the filter and its coefficients.
 */
typedef struct {
    float X;    //<- Actual input sample at (k)
    float Y;    //<- Actual output sample (k)
    float X_1;  //<- Input sample at (k - 1)
    float Y_1;  //<- Output sample at (k - 1)
    float A1;   //<- Feedback coefficient for Y_1
    float B0;   //<- Feedforward coefficient for X and X-1
} LP_FILTER_1DEGREE;

/*
 * Initializes a first-order low-pass filter using the Bilinear Transform method.
*/
void INIT_LP_FILTER_1D(LP_FILTER_1DEGREE* filter, float CUTOFF_FREQUENCY, float SAMPLING_PERIOD);

/*
 * Initializes a first-order low-pass filter using predefined coefficients.
*/
void INIT_LP_FILTER_1D_CONSTANT(LP_FILTER_1DEGREE* filter, float A1, float B0);

/*
 * Calculates the filter output for a given input sample.
 */
void CALCULATE_LP_FILTER_1D(LP_FILTER_1DEGREE* filter, float INPUT);


#endif