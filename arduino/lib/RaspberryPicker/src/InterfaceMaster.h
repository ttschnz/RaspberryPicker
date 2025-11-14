#ifndef RASPBERRY_PICKER_INTERFACE_MASTER_H
#define RASPBERRY_PICKER_INTERFACE_MASTER_H

#include <SoftwareSerial.h>
#include <Arduino.h>

#include "Controller.h"

class BasketController;
class GripperController;

class InterfaceMaster {
    public:
        InterfaceMaster();
        template <typename T>
            void send_state(const char* key, T value) {
                String str_value = String(value); // Convert value to String internally
                String data_buffer;
                data_buffer.concat(key);
                data_buffer.concat("=");
                data_buffer.concat(str_value);
                Serial.println(data_buffer);
                //this->controller = new Controller(Controller::State::IDLE);
            };

        void listen_state_change_requests();
        void add_controllers(BasketController* basket_controller,GripperController* gripper_controller);
        Controller* controller;
    private:
        //SoftwareSerial* Serial;
        BasketController* basket_controller;
        GripperController* gripper_controller;
};

#endif
