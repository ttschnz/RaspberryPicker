
#include "Basket/Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

#include "Controller.h"
#include "Gripper/Gripper.h"
#include "Gripper/GrabberStepper.h"
#include "InterfaceMaster.h"

#include <SoftwareSerial.h>
#include <Arduino.h>

InterfaceMaster::InterfaceMaster(){
    this->basket_controller = nullptr;
    this->gripper_controller = nullptr;
};

void InterfaceMaster::listen_state_change_requests(){
    while (Serial.available() > 0) {
        String line = Serial.readStringUntil('\n');  // Read a line ending with newline
        line.trim(); // Remove any trailing whitespace or \r
        int delimiterPos = line.indexOf('=');
        if (delimiterPos > 0) {
            String key = line.substring(0, delimiterPos);
            String value = line.substring(delimiterPos + 1);
            if (key == "basket.door.state"){
                DoorState new_door_state;
                this->controller->set_state(Controller::State::MANUAL);
                if (this->basket_controller && str_to_door_state(value, &new_door_state)){
                    this->basket_controller->set_door(new_door_state);
                }
            }else if (key == "basket.sorting.state"){
                SortingState new_sorting_state;
                this->controller->set_state(Controller::State::MANUAL);
                if (this->basket_controller && str_to_sorting_state(value, &new_sorting_state)){
                    this->basket_controller->set_sorting(new_sorting_state);
                }
            }else if (key == "gripper.grabber_state"){
                GrabberState new_grabber_state;
                this->controller->set_state(Controller::State::MANUAL);
                if (this->gripper_controller && str_to_grabber_state(value, &new_grabber_state)){
                    this->gripper_controller->set_grabber(new_grabber_state);
                }
            }else if (key == "controller.program"){
                Controller::Program program;
                if (this->controller && this->controller->deserialize_program(value, &program)){
                    this->controller->set_program(program);
                }
            }else if (key == "controller.state"){
                Controller::State state;
                if (this->controller && this->controller->deserialize_state(value, &state)){
                    this->controller->set_state(state);
                }
            } else{
                // other keys are read only
            }
        }
    }
}

void InterfaceMaster::add_controllers(BasketController* basket_controller, GripperController* gripper_controller){
    this->basket_controller = basket_controller;
    this->gripper_controller = gripper_controller;
}
