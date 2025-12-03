/**
 * LimitSwitch.cpp
 * 
 * Simple limit switch interface implementation.
 * Provides digital input reading for detecting physical contact/touch.
 */

#include <Arduino.h>
#include "LimitSwitch.h"

/**
 * Constructor - initializes limit switch on specified pin.
 * @param pin Digital pin number for the limit switch
 */
LimitSwitch::LimitSwitch(int pin)
{
    this->pin = pin;
    pinMode(this->pin, INPUT);  // Configure as input without pull-up
}

/**
 * Checks if the limit switch is currently activated.
 * @return true if switch is pressed/touching (HIGH signal), false otherwise
 */
bool LimitSwitch::is_touching()
{
    return digitalRead(this->pin) == HIGH;
}
