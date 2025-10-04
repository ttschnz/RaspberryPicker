#include <Arduino.h>

#include <Basket.h>
#include <Gripper.h>
#include <InterfaceSender.h>

BasketPinout basket_pinout {
  .sorting_pin = 9,
  .door_pin = 10,
};

GripperPinout gripper_pinout {
    .color_sensor_pin = 11,
    .pressure_sensor_pin = 12,
    .resistance_sensor_pin = 13,
    .stepper_motor_pins = {14,15,16,17},
};

InterfaceConfiguration interface_configuration {
    .rx_pin = 3,
    .tx_pin = 4,
    .baud_rate = 9600,
};

BasketController* basket_controller;
GripperController* gripper_controller;
InterfaceSender* interface_sender;

void setup() {
  interface_sender = new InterfaceSender(&interface_configuration);

  basket_controller = new BasketController(&basket_pinout, interface_sender);
  gripper_controller = new GripperController(&gripper_pinout, interface_sender);

  interface_sender->add_controllers(basket_controller, gripper_controller);
}

void loop() {
    interface_sender->listen_state_change_requests();
    basket_controller->set_door(DoorState::CLOSED);
    basket_controller->set_sorting(SortingState::IDLE);
    delay(1000);
    basket_controller->set_sorting(SortingState::LARGE);
    delay(1000);
    basket_controller->set_sorting(SortingState::IDLE);
    delay(1000);
    basket_controller->set_sorting(SortingState::SMALL);
    delay(1000);
    basket_controller->increment_counter();
    delay(100);
    basket_controller->increment_counter();
    delay(100);
    basket_controller->increment_counter();
    delay(100);
    basket_controller->increment_counter();
    delay(100);
    basket_controller->increment_counter();
    delay(100);
    basket_controller->increment_counter();
    delay(100);
    basket_controller->increment_counter();
    delay(1000);
    basket_controller->set_door(DoorState::OPEN_LARGE);
    basket_controller->reset_counter();
    delay(1000);
    basket_controller->set_door(DoorState::CLOSED);
    basket_controller->reset_counter();
    delay(1000);
    basket_controller->set_door(DoorState::OPEN_SMALL);
    basket_controller->reset_counter();
    delay(1000);
    basket_controller->set_door(DoorState::CLOSED);
    basket_controller->reset_counter();
    delay(1000);
}
