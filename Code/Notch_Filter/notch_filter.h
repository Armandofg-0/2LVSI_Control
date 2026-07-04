#ifndef NOTCH_FILTER_H
#define NOTCH_FILTER_H

/*
 * Structure representing a notch filter.
 *
 * Stores the internal state of the filter and its coefficients.
 */
typedef struct {
    double X;    ///< Actual input sample
    double Y;    ///< Actual output sample
    double X_1;  ///< Input sample at (k-1)
    double Y_1;  ///< Output sample at (k-1)
    double X_2;  ///< Input sample at (k-2)
    double Y_2;  ///< Output sample at (k-2)
    double A1;   ///< Feedback coefficient for Y_1
    double A2;   ///< Feedback coefficient for Y_2
    double B0;   ///< Feedforward coefficient for X
    double B1;   ///< Feedforward coefficient for X_1
    double B2;   ///< Feedforward coefficient for X_2
} NOTCH_FILTER;

/*
 * Initializes a digital notch filter using its structure.
*/
void INIT_NOTCH_FILTER(NOTCH_FILTER* filter, double CENTRAL_FREQUENCY, double SAMPLING_PERIOD);

/*
 * Calculates the filter output for a given input sample.
 */
void CALCULATE_FILTER(NOTCH_FILTER* filter, double INPUT);

#endif