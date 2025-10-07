#include <Arduino.h>
#include <avr/pgmspace.h>
#include "gaussian_pdf.h"

float gaussian_pdf(float z){
    return (1.0 / sqrt(2 * PI)) * pow(EULERS_NUMBER, -0.5 * z * z);
}
