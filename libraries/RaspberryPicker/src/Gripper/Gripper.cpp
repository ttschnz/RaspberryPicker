#include <Stepper.h>
#include <Arduino.h>

#include "../Math/gaussian_pdf.h"
#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "Gripper.h"
#include "ColorSensor.h"
#include "GripperStepper.h"
#include "PressureSensor.h"



const int ColorSensor::measure_count = 100;
const int ColorSensor::delay_probe = 10;
const int ColorSensor::delay_color = 100;
const int ColorSensor::delay_calibrate = 2000;

const float GripperStepper::transmission_ratio = 1.5 * 18 * PI;
const int GripperStepper::measuring_interval = 1;
const int GripperStepper::plate_distance = 90;
const int GripperStepper::plate_distance_min = 5;
const int GripperStepper::steps_per_revolution = 300;
const int GripperStepper::speed = 300;
const float PressureSensor::berry_size_small_standard_deviation = 0;
const float PressureSensor::berry_size_small_mean = 0;
const float PressureSensor::berry_size_large_standard_deviation = 0;
const float PressureSensor::berry_size_large_mean = 0;
const int PressureSensor::pressure_sensor_thresholds[2] = {1023,1023};

GripperController::GripperController(GripperPinout *pinout, InterfaceMaster *interface){
    this->interface = interface;
    this->gripper_state = GripperStepper::GripperState::OPEN;
    this->color_sensor = new ColorSensor(pinout->color_sensor_pinout);
    this->pressure_sensor = new PressureSensor(pinout->pressure_sensor_pins);
    this->plate_distance = GripperStepper::plate_distance;
    this->plate_stepper = new Stepper(
        GripperStepper::steps_per_revolution,
        pinout->stepper_motor_pins[0],
        pinout->stepper_motor_pins[1],
        pinout->stepper_motor_pins[2],
        pinout->stepper_motor_pins[3]
    );

    this->plate_stepper->setSpeed(GripperStepper::speed);
}



GripperStepper::RaspberrySize GripperController::set_gripper(GripperStepper::GripperState desired_gripper_state){
    // [stp] = [mm]/([mm/rot]*[rot/stp])
    int steps_per_interval =
        GripperStepper::measuring_interval / (   // [mm]
            GripperStepper::transmission_ratio * // [mm/rot]
            GripperStepper::steps_per_revolution // [stp/rot]
        );

    // [ms/stp] = 3600[ms/min]/([stp/rot]*[rot/min])
    int ms_per_step =
        3600 / (
            GripperStepper::speed *              // [rot/min]
            GripperStepper::steps_per_revolution // [stp/rot]
        );

    switch (desired_gripper_state){
        case GripperStepper::GripperState::OPEN:
            {
                int steps_til_open =
                    GripperStepper::steps_per_revolution /
                    GripperStepper::transmission_ratio * (this->plate_distance - GripperStepper::plate_distance);
                Serial.println((String)+"Steps until open: " + steps_til_open);

                this->plate_stepper->step(steps_til_open);

                delay(abs(ms_per_step * steps_til_open));

                this->plate_distance = this->plate_distance -
                    steps_til_open *  // [stp]
                    GripperStepper::transmission_ratio /     // [mm/rot]
                    GripperStepper::steps_per_revolution;    // [rot/stp]

                this->gripper_state = GripperStepper::GripperState::OPEN;
                this->interface->send_state("gripper.gripper_state", GripperStepper::serialize_gripper_state(this->gripper_state));
                this->interface->send_state("gripper.plate_distance", this->plate_distance);
            }
            break;
        case GripperStepper::GripperState::CLOSED:
            {
                // close until the plates get touch feedback
                int steps = 0;
                while(!this->pressure_sensor->is_touching() && this->plate_distance >= GripperStepper::plate_distance_min){

                    this->plate_stepper->step(steps_per_interval);

                    steps++;

                    // [mm] = [stp] * [mm/rot] / [stp/rot]
                    this->plate_distance = GripperStepper::plate_distance -
                        steps *  // [stp]
                        GripperStepper::transmission_ratio /     // [mm/rot]
                        GripperStepper::steps_per_revolution;    // [rot/stp]
                    this->interface->send_state("gripper.plate_distance", this->plate_distance);

                    delay(abs(ms_per_step * steps_per_interval));
                }

                this->gripper_state = GripperStepper::GripperState::CLOSED;
                this->interface->send_state("gripper.gripper_state", GripperStepper::serialize_gripper_state(this->gripper_state));

                float raspberry_width = this->plate_distance;

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
                    return GripperStepper::RaspberrySize::LARGE;
                }else{
                    return GripperStepper::RaspberrySize::SMALL;
                }
            }
            break;
    }

}

bool GripperController::is_ripe(){
    RGB color = this->color_sensor->measure_rgb();

    this->interface->send_state("gripper.ripeness.r", color.r);
    this->interface->send_state("gripper.ripeness.g", color.g);
    this->interface->send_state("gripper.ripeness.b", color.b);

    // TODO: linear regression?
    // for now: if the red value is higher than both other values combined (*tolerance)
    // the raspberry is considered ripe.
    return color.r > (color.b + color.g) * 0.8;
}
