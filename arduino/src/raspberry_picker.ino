#include <Arduino.h>

#include <InterfaceMaster.h>
#include <Controller.h>

#include <Basket/Basket.h>
#include <Gripper/Gripper.h>

#include <Gripper/ColorSensor.h>

BasketPinout basket_pinout {
  .sorting_pin = 13,
  .door_pin = 12,
};

GripperPinout gripper_pinout {
    .color_sensor_pinout = ColorSensor::Pinout{7,6,5,A5},
    .pressure_sensor_pins = {A4},
    .stepper_motor_pins = {8,9,10,11},
    .limit_switch_pin = 4,
};

BasketController* basket_controller;
GripperController* gripper_controller;
InterfaceMaster* interface_master;
Controller* controller;

void setup() {

  Serial.begin(9600);
    while(!Serial){};

  Serial.println("initialising");

  interface_master = new InterfaceMaster();
  Serial.println("interface ready");

  controller = new Controller(Controller::State::IDLE, interface_master);
  Serial.println("controller created");

  basket_controller = new BasketController(&basket_pinout, interface_master);
  Serial.println("basket controller ready");
  gripper_controller = new GripperController(&gripper_pinout, interface_master);
  Serial.println("gripper controller ready");

  interface_master->add_controllers(basket_controller, gripper_controller);
  interface_master->controller = controller;
  Serial.println("controller connected to interface");
  controller->add_controllers(basket_controller, gripper_controller);
  Serial.println("controllers connected to main controller");
  controller->add_interface(interface_master);
  Serial.println("interface connected to main controller");
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
        case Controller::Program::CLOSE_GRIPPER:
          controller->run_close();
          break;
        case Controller::Program::RELEASE_GRIPPER:
          controller->run_release();
          break;
        case Controller::Program::EMPTY_BASKET:
          controller->run_drop();
          break;
        case Controller::Program::RESET:
          controller->run_reset();
          break;
        case Controller::Program::CALIBRATE_COLOR:
          Serial.println("running calibrate color");
          controller->run_calibrate_color();
          break;
      }
      controller->set_state(Controller::State::IDLE);
      break;
  }
    delay(100); // for stability
}
