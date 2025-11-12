#include <Arduino.h>
#include "LimitSwitch.h"

LimitSwitch::LimitSwitch(int pin){
    this->pin = pin;
    pinMode(this->pin, INPUT_PULLUP);
}

bool LimitSwitch::is_touching(){
    // we use internal pullup resistor and connect pin to gnd via switch. 
    // if the switch is closed, we have a LOW reading since we are grounded
    return digitalRead(this->pin) == LOW;
}
