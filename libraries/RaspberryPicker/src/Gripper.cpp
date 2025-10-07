#include <Stepper.h>
#include <Arduino.h>

#include "Gripper/ColorSensor.h"
//#include "Gripper/PressureSensor.h"
#include "Gripper/GrabberStepper.h"

#include "Gripper.h"


const float ColorSensorValues::threshold = 0.5;

const float GrabberStepperMotorValues::transmission_ratio = 0.5;
const int GrabberStepperMotorValues::measuring_interval = 1;
const int GrabberStepperMotorValues::plate_distance = 30;
const int GrabberStepperMotorValues::steps_per_revolution = 300;
const int GrabberStepperMotorValues::speed = 300;

GripperController::GripperController(GripperPinout *pinout,InterfaceSender *interface){
    this->interface = interface;

    pinMode(pinout->color_sensor_pin, INPUT);
    pinMode(pinout->pressure_sensor_pin, INPUT);
    pinMode(pinout->resistance_sensor_pin, INPUT);

    this->plate_stepper = new Stepper(
        PlateStepperMotorValues::steps_per_revolution,
        pinout->stepper_motor_pins[0],
        pinout->stepper_motor_pins[1],
        pinout->stepper_motor_pins[2],
        pinout->stepper_motor_pins[3]
    );

    this->plate_stepper->setSpeed(GrabberStepperMotorValues::speed);
}


RaspberrySize GripperController::set_grabber(GrabberState desired_grabber_state){
    // close until the plates get touch feedback
    return RaspberrySize::LARGE;
}

bool measure_color(){
    return true; // allways ripe
}
