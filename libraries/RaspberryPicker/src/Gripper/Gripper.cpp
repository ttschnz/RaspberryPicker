#include <Stepper.h>
#include <Arduino.h>

#include "../Math/gaussian_pdf.h"
#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "Gripper.h"
#include "ColorSensor.h"
#include "GrabberStepper.h"
#include "PressureSensor.h"



const float ColorSensorValues::threshold = 0.5;

const float GrabberStepperMotorValues::transmission_ratio = 1.5 * 18 * PI;
const int GrabberStepperMotorValues::measuring_interval = 1;
const int GrabberStepperMotorValues::plate_distance = 90;
const int GrabberStepperMotorValues::steps_per_revolution = 300;
const int GrabberStepperMotorValues::speed = 300;
const float PressureSensor::berry_size_small_standard_deviation = 0;
const float PressureSensor::berry_size_small_mean = 0;
const float PressureSensor::berry_size_large_standard_deviation = 0;
const float PressureSensor::berry_size_large_mean = 0;
const int PressureSensor::pressure_sensor_thresholds[2] = {1023,1023};

GripperController::GripperController(GripperPinout *pinout, InterfaceMaster *interface){
    this->interface = interface;
    pinMode(pinout->color_sensor_pin, INPUT);
    pinMode(pinout->resistance_sensor_pin, INPUT);

    this->pressure_sensor = new PressureSensor(pinout->pressure_sensor_pins);

    this->plate_stepper = new Stepper(
        GrabberStepperMotorValues::steps_per_revolution,
        pinout->stepper_motor_pins[0],
        pinout->stepper_motor_pins[1],
        pinout->stepper_motor_pins[2],
        pinout->stepper_motor_pins[3]
    );

    this->plate_stepper->setSpeed(GrabberStepperMotorValues::speed);
}

const char* raspberry_size_to_str(RaspberrySize raspberry_size){
    int idx = (int)raspberry_size;
    return raspberry_size_strings[idx];
}

bool str_to_raspberry_size(String raspberry_size_str, RaspberrySize* out_raspberry_size){
    bool matched = true;
    if (raspberry_size_str=="LARGE"){
        *out_raspberry_size = RaspberrySize::LARGE;
    }else if (raspberry_size_str=="SMALL"){
        *out_raspberry_size = RaspberrySize::SMALL;
    }else{
        matched = false;
    }
    return matched;
}


RaspberrySize GripperController::set_grabber(GrabberState desired_grabber_state){
    // close until the plates get touch feedback

    // [stp] = [mm]/([mm/rot]*[rot/stp])
    int steps_per_interval =
        GrabberStepperMotorValues::measuring_interval / (   // [mm]
            GrabberStepperMotorValues::transmission_ratio * // [mm/rot]
            GrabberStepperMotorValues::steps_per_revolution // [stp/rot]
        );

    // [ms/stp] = 3600[ms/min]/([stp/rot]*[rot/min])
    int ms_per_step =
        3600 / (
            GrabberStepperMotorValues::speed *              // [rot/min]
            GrabberStepperMotorValues::steps_per_revolution // [stp/rot]
        );

    int steps = 0;
    while(!this->pressure_sensor->is_touching()){
        this->plate_stepper->step(steps_per_interval);
        steps++;
        delay(ms_per_step * steps_per_interval);
    }

    // [mm] = [stp] * [mm/rot] / [stp/rot]
    float raspberry_width = GrabberStepperMotorValues::plate_distance -
        steps *  // [stp]
        GrabberStepperMotorValues::transmission_ratio /     // [mm/rot]
        GrabberStepperMotorValues::steps_per_revolution;    // [rot/stp]

    // z = (x - mean) / stdev
    float z_large = (
            raspberry_width-PressureSensor::berry_size_large_mean
        ) / PressureSensor::berry_size_large_standard_deviation;

    float z_small = (
            raspberry_width-PressureSensor::berry_size_small_mean
        ) / PressureSensor::berry_size_small_standard_deviation;

    float p_w_given_L = gaussian_pdf(z_large);
    float p_w_given_S = gaussian_pdf(z_small);

    float p_L = 0.5, p_S = 0.5;

    float probability_large = p_w_given_L * p_L / (p_w_given_L * p_L + p_w_given_S * p_S);
    float probability_small = p_w_given_S * p_L / (p_w_given_L * p_L + p_w_given_S * p_S);

    this->interface->send_state("gripper.berry_p_large", probability_large);
    this->interface->send_state("gripper.berry_p_small", probability_small);

    if (probability_large>probability_small){
        return RaspberrySize::LARGE;
    }else{
        return RaspberrySize::SMALL;
    }
}

bool GripperController::measure_color(){
    return true; // allways ripe
}
