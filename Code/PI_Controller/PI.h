#ifndef PI_H
#define PI_H

/*
 * Structure representing a discrete-time PI (Proportional-Integral) controller.
 *
 * Stores internal state and coefficients used in the controller's difference equation.
 */
typedef struct {
    float error;     //<- Actual control error (k)
    float error_1;   //<- Previous control error (k-1)
    float out;       //<- Actual output (k)
    float out_1;     //<- Previous output (k-1)
    float B0;        //<- Coefficient for error (k)
    float B1;        //<- Coefficient for error (k-1)
    float LIMIT;     //<- Output saturation limit (absolute)
} PI_FORM;

/*
 * Initializes the PI controller: computes coefficients and resets internal state.
 */
void INIT_PI(PI_FORM* PI, float K_p, float K_i, float SAMPLING_PERIOD, float LIMIT);

/*
 * Calculates the PI control output using a discrete-time difference equation.
 * Applies output saturation.
 */
void CALCULATE_PI(PI_FORM *PI, float error);

/*
 * Calculates the PI control output with a feedforward term (K_ff * reference).
 * Applies saturation and updates internal state to limit windup.
 */
void CALCULATE_PI_FF(PI_FORM *PI, float REFERENCIA, float error, float K_ff);

#endif