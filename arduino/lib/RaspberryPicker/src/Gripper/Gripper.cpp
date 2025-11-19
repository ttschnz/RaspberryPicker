#include <AccelStepper.h>
#include <Arduino.h>

#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "Gripper.h"
#include "ColorSensor.h"
#include "GripperStepper.h"
#include "PressureSensor.h"
#include "LimitSwitch.h"


const int ColorSensor::measure_count = 100;
const int ColorSensor::delay_probe = 10;
const int ColorSensor::delay_color = 100;
const int ColorSensor::delay_calibrate = 2000;

const float GripperStepper::transmission_ratio = 1.5 * 18 * PI;
const int GripperStepper::plate_distance_open = 65;
const int GripperStepper::plate_distance_large = 30;
const int GripperStepper::plate_distance_small = 20;
const int GripperStepper::plate_distance_limit = 15;
const int GripperStepper::steps_per_revolution = 2048*2;
const int GripperStepper::speed = 200;
const int GripperStepper::max_speed = 1200;
const int GripperStepper::acceleration = 300;
const float PressureSensor::berry_size_threshold = 25;
const int PressureSensor::pressure_sensor_thresholds[2] = {5,5};

GripperController::GripperController(GripperPinout *pinout, InterfaceMaster *interface){
    this->interface = interface;
    this->gripper_state = GripperStepper::GripperState::OPEN;
    this->color_sensor = new ColorSensor(pinout->color_sensor_pinout);
    this->pressure_sensor = new PressureSensor(pinout->pressure_sensor_pins);
    this->plate_distance = GripperStepper::plate_distance_open;
    this->limit_switch = new LimitSwitch(pinout->limit_switch_pin);
    this->plate_stepper = new AccelStepper(
        AccelStepper::HALF4WIRE,
        pinout->stepper_motor_pins[0],
        pinout->stepper_motor_pins[2],
        pinout->stepper_motor_pins[1],
        pinout->stepper_motor_pins[3]
    );

    this->plate_stepper->setCurrentPosition(
        GripperStepper::get_desired_step_position(GripperStepper::GripperState::OPEN)
    );
    this->plate_stepper->setSpeed(GripperStepper::speed);
    this->plate_stepper->setMaxSpeed(GripperStepper::max_speed);
    this->plate_stepper->setAcceleration(GripperStepper::acceleration);

}



GripperStepper::RaspberrySize GripperController::set_gripper(GripperStepper::GripperState desired_gripper_state){
    int target_steps = GripperStepper::get_desired_step_position(desired_gripper_state);
    switch (desired_gripper_state){
        case GripperStepper::GripperState::OPEN:
            {
                this->plate_stepper->moveTo(target_steps);
                int i = 0;
                while (this->plate_stepper->isRunning()){
                    this->plate_stepper->run();
                    i++;
                    if(i>10000){
                        i=0;
                        int current_position_step = this->plate_stepper->currentPosition();
                        this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
                        this->interface->send_state("gripper.plate_distance", this->plate_distance);
                    }
                }

                int current_position_step = this->plate_stepper->currentPosition();
                this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
                this->gripper_state = GripperStepper::GripperState::OPEN;
                this->interface->send_state("gripper.gripper_state", GripperStepper::serialize_gripper_state(this->gripper_state));
                this->interface->send_state("gripper.plate_distance", this->plate_distance);
                return GripperStepper::RaspberrySize::UNKNOWN;
            }
            break;
        case GripperStepper::GripperState::CLOSED_LIMIT:
            {
                this->plate_stepper->moveTo(target_steps);
                bool limit_switch = false;
                int i = 0;
                do{
                    this->plate_stepper->run();

                    i++;
                    if(i>10000){
                        i=0;
                        int current_position_step = this->plate_stepper->currentPosition();
                        this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
                        this->interface->send_state("gripper.plate_distance", this->plate_distance);
                    }

                    limit_switch = this->limit_switch->is_touching();

                } while(!limit_switch && this->plate_stepper->isRunning());

                if (!limit_switch){
                    // the stepper reached the "minimal" position.
                    // we continue at low speed until we hit the limit switch
                    Serial.println((String)+"closed without reaching limit switch. finding zero");
                    this->plate_stepper->setSpeed(-GripperStepper::speed); // negative for closing
                    while(!this->limit_switch->is_touching()){
                        this->plate_stepper->runSpeed();
                    }
                }
                this->plate_stepper->setCurrentPosition(
                    GripperStepper::get_desired_step_position(GripperStepper::GripperState::CLOSED_LIMIT)
                );
                return GripperStepper::RaspberrySize::UNKNOWN;
            }
            break;
        case GripperStepper::GripperState::CLOSED_LARGE:
        case GripperStepper::GripperState::CLOSED_SMALL:
            {                
                this->plate_stepper->moveTo(target_steps);
                bool touching = false;
                bool limit_switch = false;
                int i = 0;
                do{
                    this->plate_stepper->run();

                    i++;
                    if(i>10000){
                        i=0;
                        int current_position_step = this->plate_stepper->currentPosition();
                        this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
                        this->interface->send_state("gripper.plate_distance", this->plate_distance);
                    }

                    touching = this->pressure_sensor->is_touching(i%1000==0);
                    limit_switch = this->limit_switch->is_touching();

                } while(!touching && !limit_switch && this->plate_stepper->isRunning());


                GripperStepper::RaspberrySize size;
                GripperStepper::GripperState state;
                
                if (touching){
                    this->plate_stepper->stop();
                    //while (this->plate_stepper->isRunning()) this->plate_stepper->run(); // decelerate cleanly


                    int current_position_step = this->plate_stepper->currentPosition();
                    int raspberry_width = GripperStepper::steps_to_mm(current_position_step);

                    if (raspberry_width > PressureSensor::berry_size_threshold){
                        size = GripperStepper::RaspberrySize::LARGE;
                        state = GripperStepper::GripperState::CLOSED_LARGE;
                    }else{
                        size = GripperStepper::RaspberrySize::SMALL;
                        state = GripperStepper::GripperState::CLOSED_SMALL;
                    }
                }else{
                    state = desired_gripper_state;
                    size = GripperStepper::RaspberrySize::UNKNOWN;
                }

                this->gripper_state = state;
                this->interface->send_state("gripper.gripper_state",
                    GripperStepper::serialize_gripper_state(state)
                );

                return size;

            }
            break;
    }
    return GripperStepper::RaspberrySize::UNKNOWN;
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
