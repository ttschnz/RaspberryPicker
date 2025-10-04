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

InterfacePinout interface_pinout {
    .rx = 3,
    .tx = 4,
};

BasketController* basket_controller;
GripperController* gripper_controller;
InterfaceSender* interface_sender;

void setup() {
  basket_controller = new BasketController(&basket_pinout);
  gripper_controller = new GripperController(&gripper_pinout);
  interface_sender = new InterfaceSender(&interface_pinout);
}

void loop() {

}
