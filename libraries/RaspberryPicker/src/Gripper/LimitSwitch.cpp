#include <Arduino.h>
#include "LimitSwitch.h"

LimitSwitch::LimitSwitch(int pin){
    this->pin = pin;
    pinMode(this->pin, INPUT);
}

bool LimitSwitch::is_touching(){
    return digitalRead(this->pin) == HIGH;
}
