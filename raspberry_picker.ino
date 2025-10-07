#include <Arduino.h>

#include <InterfaceMaster.h>
#include <Controller.h>

#include <Basket/Basket.h>
#include <Gripper/Gripper.h>

BasketPinout basket_pinout {
  .sorting_pin = 9,
  .door_pin = 10,
};

GripperPinout gripper_pinout {
    .color_sensor_pin = 11,
    .pressure_sensor_pins = {12,13},
    .resistance_sensor_pin = 18,
    .stepper_motor_pins = {14,15,16,17},
};

InterfaceConfiguration interface_configuration {
    .rx_pin = 3,
    .tx_pin = 4,
    .baud_rate = 9600,
};

BasketController* basket_controller;
GripperController* gripper_controller;
InterfaceMaster* interface_master;
Controller* controller;

void setup() {
  controller = new Controller(Controller::State::IDLE);

  interface_master = new InterfaceMaster(&interface_configuration);

  basket_controller = new BasketController(&basket_pinout, interface_master);
  gripper_controller = new GripperController(&gripper_pinout, interface_master);

  interface_master->add_controllers(basket_controller, gripper_controller);
  controller->add_controllers(basket_controller, gripper_controller);
}

void loop() {
    switch (controller->get_state()){
        case Controller::State::IDLE:
            interface_master->listen_state_change_requests();
            break;
        case Controller::State::MANUAL:
            interface_master->listen_state_change_requests();
            break;
        case Controller::State::PROGRAM:
            interface_master->listen_state_change_requests();
            switch (controller->get_program()){
                case Controller::Program::CLOSE:
                    controller->run_close();
                    break;
                case Controller::Program::DROP:
                    controller->run_drop();
                    break;
                case Controller::Program::RESET:
                    controller->run_reset();
                    break;
            }
            break;
    }
}
