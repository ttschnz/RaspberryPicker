#include <Stepper.h>
#include <Arduino.h>

#include "Gripper/ColorSensor.h"
//#include "Gripper/PressureSensor.h"
#include "Gripper/ResistanceSensor.h"
#include "Gripper/PlateStepper.h"

#include "Gripper.h"


const float ColorSensorValues::threshold = 0.5;

const float ResistanceSensorValues::threshold = 0.5;

const float PlateStepperMotorValues::transmission_ratio = 0.5;
const int PlateStepperMotorValues::measuring_interval = 1;
const int PlateStepperMotorValues::plate_distance = 30;
const int PlateStepperMotorValues::steps_per_revolution = 300;
const int PlateStepperMotorValues::speed = 300;

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

    this->plate_stepper->setSpeed(PlateStepperMotorValues::speed);
}
