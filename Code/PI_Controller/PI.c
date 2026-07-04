#include "PI.h"

/*
 * Initializes a discrete-time PI (Proportional-Integral) controller.
 *
 * This function calculates the controller coefficients using the Tustin approximation
 * and sets the initial internal states to zero. It also applies an output limit to avoid saturation.
 *
 * Parameters:
 *   PI               - Pointer to the PI_FORM structure to be initialized.
 *   K_p              - Proportional gain.
 *   K_i              - Integral gain.
 *   SAMPLING_PERIOD  - Sampling period of the control loop (in seconds).
 *   LIMIT            - Maximum absolute value allowed for the controller output.
 */
void INIT_PI(PI_FORM* PI, float K_p, float K_i, float SAMPLING_PERIOD, float LIMIT){
    
    // Initialize internal states
    PI->error = 0;
    PI->error_1 = 0;
    PI->out = 0;
    PI->out_1 = 0;

    // Set output limit
    PI->LIMIT = LIMIT;

    // Compute PI controller coefficients using Tustin approximation
    PI->B0 = (2.0 * K_p + K_i * SAMPLING_PERIOD) / 2.0;
    PI->B1 = (K_i * SAMPLING_PERIOD - 2.0 * K_p) / 2.0;
}

/*
 * Calculates the output of a discrete-time PI (Proportional-Integral) controller.
 *
 * This function implements a PI control law using a difference equation with 
 * precomputed coefficients (B0 and B1). It also applies output saturation to 
 * keep the controller output within defined limits.
 * 
 * Parameters:
 * PI         - Pointer to the PI_FORM structure containing controller state and parameters.
 * REFERENCIA - Desired reference value (setpoint).
 * MEAS       - Current measured process value (feedback).
 */
void CALCULATE_PI(PI_FORM *PI, float error){

    float out_temp;

    // Compute temporary output using the discrete PI difference equation
    out_temp = PI->B0 * error + PI->B1 * PI->error_1 + PI->out_1;

    // Apply output saturation (limit the control output)
    if (out_temp > PI->LIMIT){
        PI->out = PI->LIMIT;
    }
    else if (out_temp < - PI->LIMIT){
        PI->out = - PI->LIMIT;
    }
    else { 
        PI->out = out_temp;
    }

    // Update previous output and error for the next control cycle
    PI->out_1 = PI->out;
    PI->error_1 = error;
}

/*
 * Calculates the output of a discrete-time PI controller with feedforward compensation.
 *
 * This function extends a standard PI controller by adding a feedforward term (K_ff * REFERENCIA)
 * 
 * Parameters:
 * PI         - Pointer to the PI_FORM structure containing controller state and parameters.
 * REFERENCIA - Desired reference value (setpoint).
 * MEAS       - Current measured process value (feedback).
 * K_ff       - Feedforward gain applied to the reference signal.
 */
void CALCULATE_PI_FF(PI_FORM *PI, float REFERENCIA, float error, float K_ff){

    float out_temp;

    // Compute raw PI output with feedforward term
    out_temp = (PI->B0 * error) + (PI->B1 * PI->error_1) + PI->out_1 + (K_ff * REFERENCIA);

    // Apply output saturation and basic anti-windup
    if (out_temp > PI->LIMIT){
        PI->out = PI->LIMIT;
        PI->out_1 = PI->LIMIT - K_ff * REFERENCIA;
    }
    else if (out_temp < - 1.0 * PI->LIMIT){
        PI->out = -1.0 * PI->LIMIT;
        PI->out_1 = -1.0 * PI->LIMIT - K_ff * REFERENCIA;
    }
    else {
        // No saturation: update output and internal states normally
        PI->out = out_temp;
        PI->out_1 = PI->out - K_ff * REFERENCIA;
        PI->error_1 = error;
    }
}