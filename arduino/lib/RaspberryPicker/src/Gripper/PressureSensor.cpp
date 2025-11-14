#include <Arduino.h>
#include "PressureSensor.h"

PressureSensor::PressureSensor(int pinout[2]){
    this->pinout[0] = pinout[0];
    this->pinout[1] = pinout[1];

    pinMode(this->pinout[0], INPUT);
    pinMode(this->pinout[1], INPUT);
}

static const char* pressure_state_strings[] = {
    "TOUCHING",
    "NOT_TOUCHING",
};

const char* PressureSensor::serialize_pressure_state(PressureSensor::PressureState pressure_state){
    int idx = (int)pressure_state;
    return pressure_state_strings[idx];
};

bool PressureSensor::deserialize_pressure_state(String pressure_state_str, PressureSensor::PressureState* out_pressure_state){
    bool matched = true;
    if (pressure_state_str == "TOUCHING") {
        *out_pressure_state = PressureSensor::PressureState::TOUCHING;
    } else if (pressure_state_str == "NOT_TOUCHING") {
        *out_pressure_state = PressureSensor::PressureState::NOT_TOUCHING;
    } else{
        matched = false;
    }
    return matched;
}


bool PressureSensor::is_touching(){
    int voltages[2];
    voltages[0] = analogRead(this->pinout[0]);
    voltages[1] = analogRead(this->pinout[1]);

    bool touching[2];
    touching[0] = this->pressure_sensor_thresholds[0] < voltages[0];
    touching[1] = this->pressure_sensor_thresholds[1] < voltages[1];

    return touching[0] && touching[1];
}
