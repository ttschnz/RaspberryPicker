
#include "Basket/Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

#include "Controller.h"
#include "Gripper/Gripper.h"
#include "InterfaceMaster.h"

#include <SoftwareSerial.h>
#include <Arduino.h>

InterfaceMaster::InterfaceMaster(InterfaceConfiguration* config){
    pinMode(config->rx_pin, INPUT);
    pinMode(config->tx_pin, OUTPUT);

    this->Serial = new SoftwareSerial(config->rx_pin, config->tx_pin);
    this->Serial->begin(config->baud_rate);
};

void InterfaceMaster::listen_state_change_requests(){
    while (this->Serial->available() > 0) {
        String line = this->Serial->readStringUntil('\n');  // Read a line ending with newline
        line.trim(); // Remove any trailing whitespace or \r

        int delimiterPos = line.indexOf('=');
        if (delimiterPos > 0) {
            String key = line.substring(0, delimiterPos);
            String value = line.substring(delimiterPos + 1);

            if (key == "door.state"){
                DoorState new_door_state;
                if (this->basket_controller && str_to_door_state(value, &new_door_state)){
                    this->basket_controller->set_door(new_door_state);
                }
            }else if (key == "sorting.state"){
                SortingState new_sorting_state;
                if (this->basket_controller && str_to_sorting_state(value, &new_sorting_state)){
                    this->basket_controller->set_sorting(new_sorting_state);
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
