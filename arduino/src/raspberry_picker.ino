/**
 * raspberry_picker.ino
 * 
 * Main Arduino sketch for the Raspberry Picker robot system.
 * This file initializes and coordinates the basket controller, gripper controller,
 * and interface master to enable automated raspberry picking, sorting, and basket management.
 * 
 * The system operates in three states: IDLE, MANUAL, and PROGRAM.
 * Various programs are available for different operations like closing/releasing gripper,
 * emptying basket, measuring color, and executing automated picking sequences.
 */

#include <Arduino.h>

#include <InterfaceMaster.h>
#include <Controller.h>

#include <Basket/Basket.h>
#include <Gripper/Gripper.h>

#include <Gripper/ColorSensor.h>

// Pin configuration for the basket system
// sorting_pin controls the servo that directs raspberries to small/large compartments
// door_pin controls the servo that opens/closes the basket door for emptying
BasketPinout basket_pinout{
    .sorting_pin = 13,
    .door_pin = 12,
};

// Pin configuration for the gripper system
// color_sensor_pinout: RGB LED pins (R, G, B) and LDR analog pin for color detection
// stepper_motor_pins: control pins for the stepper motor that controls gripper plates
// limit_switch_zero_pin: detects when gripper is fully closed (zero position)
// limit_switch_pressure_pin: detects when a raspberry is held by the gripper
GripperPinout gripper_pinout{
    .color_sensor_pinout = ColorSensor::Pinout{7, 6, 5, A5},
    .stepper_motor_pins = {8, 9, 10, 11},
    .limit_switch_zero_pin = 4,
    .limit_switch_pressure_pin = 3,
};

// Global controller objects that manage the robot subsystems
BasketController *basket_controller;    // Manages basket door and sorting mechanism
GripperController *gripper_controller;  // Manages gripper, stepper motor, and sensors
InterfaceMaster *interface_master;      // Handles serial communication and state updates
Controller *controller;                 // Main controller coordinating all operations

/**
 * Initialization function called once at startup.
 * Sets up serial communication and initializes all controller objects.
 * Establishes connections between controllers for coordinated operation.
 */
void setup()
{
  // Initialize serial communication at 9600 baud for debugging and interface
  Serial.begin(9600);
  while (!Serial)
  {
  };

  Serial.println("initialising");

  // Create interface master for serial communication
  interface_master = new InterfaceMaster();
  Serial.println("interface ready");

  // Create main controller in IDLE state
  controller = new Controller(Controller::State::IDLE, interface_master);
  Serial.println("controller created");

  // Initialize basket controller with pin configuration
  basket_controller = new BasketController(&basket_pinout, interface_master);
  Serial.println("basket controller ready");
  
  // Initialize gripper controller with pin configuration and sensors
  gripper_controller = new GripperController(&gripper_pinout, interface_master);
  Serial.println("gripper controller ready");

  // Connect all controllers to enable coordinated operation
  interface_master->add_controllers(basket_controller, gripper_controller);
  interface_master->controller = controller;
  Serial.println("controller connected to interface");
  controller->add_controllers(basket_controller, gripper_controller);
  Serial.println("controllers connected to main controller");
  controller->add_interface(interface_master);
  Serial.println("interface connected to main controller");
}

/**
 * Main loop function that continuously executes system operations.
 * Implements a state machine with three states:
 * - IDLE: Waiting for commands, listening for state change requests
 * - MANUAL: Manual control mode, listening for state change requests
 * - PROGRAM: Executing automated programs based on selected program type
 * 
 * After completing a program, the system returns to IDLE state.
 */
void loop()
{
  switch (controller->get_state())
  {
  case Controller::State::IDLE:
    // IDLE state: listen for incoming commands via serial interface
    interface_master->listen_state_change_requests();
    break;
  case Controller::State::MANUAL:
    // MANUAL state: allow manual control of individual components
    interface_master->listen_state_change_requests();
    break;
  case Controller::State::PROGRAM:
    // PROGRAM state: execute the selected automated program
    interface_master->listen_state_change_requests();
    switch (controller->get_program())
    {
    case Controller::Program::CLOSE_GRIPPER:
      // Close gripper and detect raspberry size and ripeness
      controller->run_close();
      break;
    case Controller::Program::RELEASE_GRIPPER:
      // Open gripper and increment basket counter
      controller->run_release();
      break;
    case Controller::Program::EMPTY_BASKET:
      // Open basket door to empty contents
      controller->run_drop();
      break;
    case Controller::Program::RESET:
      // Reset all components to initial positions
      controller->run_reset();
      break;
    case Controller::Program::MEASURE_COLOR:
      // Continuously measure and report color values
      controller->run_measure_color();
      break;
    case Controller::Program::PROGRAM_1:
      // Complete picking sequence: close, measure, sort, release
      controller->run_pgm1();
      break;
    case Controller::Program::PROGRAM_2:
      // Empty basket and reset counters
      controller->run_pgm2();
      break;
    }
    // Return to IDLE state after program execution
    controller->set_state(Controller::State::IDLE);
    break;
  }
  delay(100); // Small delay for system stability
}
