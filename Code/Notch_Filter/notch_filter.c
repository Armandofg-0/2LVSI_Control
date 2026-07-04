#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "NOTCH_FILTER.h"

#define PI 3.14159265358979323846

/*
 * Initializes a second-order notch filter.
 *
 * Computes the filter coefficients using the bilinear transform based on the specified
 * central frequency and sampling period. It also clears all internal state variables.
 *
 * Parameters:
 *   filter            - Pointer to the NOTCH_FILTER structure to be initialized.
 *   CENTRAL_FREQUENCY - The frequency (in Hz) where the notch filter attenuates.
 *   SAMPLING_PERIOD   - The sampling period (in seconds).
 */
void INIT_NOTCH_FILTER(NOTCH_FILTER* filter, double CENTRAL_FREQUENCY, double SAMPLING_PERIOD) {
    // Reset de estados internos
    filter->X = 0.0; filter->X_1 = 0.0; filter->X_2 = 0.0;
    filter->Y = 0.0; filter->Y_1 = 0.0; filter->Y_2 = 0.0;

    double omega = 2.0 * PI * CENTRAL_FREQUENCY; 
    double T = SAMPLING_PERIOD;

    // Un factor de calidad (Q) de 0.707 a 1.5 es ideal para rechazar armónicos en dq.
    // Ojo: Si usas sqrt(2) está bien, pero puedes hacerlo variable si quieres variar el ancho de banda.
    double Q = 1.0; 

    // --- Deducción correcta Bilineal ---
    // Denominador común (A0) antes de normalizar
    double DENOMINADOR = 4.0 * Q + 2.0 * T * omega + Q * pow(T, 2.0) * pow(omega, 2.0);

    // Coeficientes del Numerador (B) -> El término central NO lleva el amortiguamiento de omega
    filter->B0 = (4.0 * Q + Q * pow(T, 2.0) * pow(omega, 2.0)) / DENOMINADOR;
    filter->B1 = (2.0 * Q * pow(T, 2.0) * pow(omega, 2.0) - 8.0 * Q) / DENOMINADOR;
    filter->B2 = (4.0 * Q + Q * pow(T, 2.0) * pow(omega, 2.0)) / DENOMINADOR;

    // Coeficientes del Denominador (A) -> El término central SI lleva el amortiguamiento (resta o suma según signo)
    filter->A1 = (2.0 * Q * pow(T, 2.0) * pow(omega, 2.0) - 8.0 * Q) / DENOMINADOR;
    filter->A2 = (4.0 * Q - 2.0 * T * omega + Q * pow(T, 2.0) * pow(omega, 2.0)) / DENOMINADOR;
}

/*
 * Processes a single input sample through a notch filter.
 *
 * This function applies the difference equation to compute the filtered output
 * based on the current and previous input/output samples.
 *
 * filter   - Pointer to the NOTCH_FILTER structure.
 * INPUT    - Current input sample to be filtered.
 */
void CALCULATE_FILTER(NOTCH_FILTER* filter, double INPUT){
    // 1. Almacenar copias locales de los retrasos antes de tocar la estructura
    double x0 = INPUT;
    double x1 = filter->X_1;
    double x2 = filter->X_2;
    double y1 = filter->Y_1;
    double y2 = filter->Y_2;

    // 2. Calcular la ecuación en diferencias usando estrictamente las copias locales
    double y0 = filter->B0 * x0 + filter->B1 * x1 + filter->B2 * x2 
              - filter->A1 * y1 - filter->A2 * y2;
    
    // 3. Actualizar el registro histórico en la estructura (Desplazamiento real)
    filter->X_2 = x1;  // El viejo (k-1) pasa a ser (k-2)
    filter->Y_2 = y1;  // El viejo (k-1) pasa a ser (k-2)
    
    filter->X_1 = x0;  // El actual (k) pasa a ser (k-1) para el próximo paso
    filter->Y_1 = y0;  // El actual (k) pasa a ser (k-1) para el próximo paso

    // 4. Guardar salidas actuales para lectura externa
    filter->X = x0;
    filter->Y = y0;
}
