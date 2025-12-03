/**
 * InterfaceMaster.cpp
 * 
 * Serial communication interface implementation for the Raspberry Picker.
 * Handles bidirectional communication:
 * - Receives commands from external control systems via serial
 * - Sends state updates and sensor data back via serial
 * - Parses key=value pairs for state changes
 * - Routes commands to appropriate controllers
 */

#include "Basket/Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

#include "Controller.h"
#include "Gripper/Gripper.h"
#include "Gripper/GripperStepper.h"
#include "InterfaceMaster.h"

#include <SoftwareSerial.h>
#include <Arduino.h>

/**
 * Constructor - initializes interface with null controller references.
 * Controllers must be added via add_controllers() before use.
 */
InterfaceMaster::InterfaceMaster()
{
    this->basket_controller = nullptr;
    this->gripper_controller = nullptr;
};

/**
 * Listens for and processes state change requests from serial interface.
 * 
 * Expects commands in format: key=value
 * Supported keys:
 * - basket.door.state: Control basket door (OPEN/CLOSED)
 * - basket.sorting.state: Control sorting mechanism (IDLE/SMALL/LARGE)
 * - gripper.gripper_state: Control gripper (OPEN/CLOSED_SMALL/CLOSED_LARGE/CLOSED_LIMIT)
 * - controller.program: Set program to execute
 * - controller.state: Set controller state (IDLE/MANUAL/PROGRAM)
 * 
 * Most commands automatically switch controller to MANUAL mode.
 */
void InterfaceMaster::listen_state_change_requests()
{
    while (Serial.available() > 0)
    {
        // Read line from serial (newline-terminated)
        String line = Serial.readStringUntil('\n');
        line.trim();  // Remove whitespace and carriage return
        
        // Parse key=value format
        int delimiterPos = line.indexOf('=');
        if (delimiterPos > 0)
        {
            String key = line.substring(0, delimiterPos);
            String value = line.substring(delimiterPos + 1);
            
            // Route command to appropriate controller based on key
            if (key == "basket.door.state")
            {
                BasketDoor::DoorState new_door_state;
                this->controller->set_state(Controller::State::MANUAL);
                if (this->basket_controller && BasketDoor::deserialize_door_state(value, &new_door_state))
                {
                    this->basket_controller->set_door(new_door_state);
                }
            }
            else if (key == "basket.sorting.state")
            {
                BasketSorter::SortingState new_sorting_state;
                this->controller->set_state(Controller::State::MANUAL);
                if (this->basket_controller && BasketSorter::deserialize_sorting_state(value, &new_sorting_state))
                {
                    this->basket_controller->set_sorting(new_sorting_state);
                }
            }
            else if (key == "gripper.gripper_state")
            {
                GripperStepper::GripperState new_gripper_state;
                this->controller->set_state(Controller::State::MANUAL);
                if (this->gripper_controller && GripperStepper::deserialize_gripper_state(value, &new_gripper_state))
                {
                    this->gripper_controller->set_gripper(new_gripper_state);
                }
            }
            else if (key == "controller.program")
            {
                Controller::Program program;
                if (this->controller && this->controller->deserialize_program(value, &program))
                {
                    this->controller->set_program(program);
                }
            }
            else if (key == "controller.state")
            {
                Controller::State state;
                if (this->controller && this->controller->deserialize_state(value, &state))
                {
                    this->controller->set_state(state);
                }
            }
            else
            {
                // Unknown or read-only key - ignore
            }
        }
    }
}

/**
 * Adds references to basket and gripper controllers.
 * Must be called during initialization to enable command routing.
 * @param basket_controller Pointer to BasketController
 * @param gripper_controller Pointer to GripperController
 */
void InterfaceMaster::add_controllers(BasketController *basket_controller, GripperController *gripper_controller)
{
    this->basket_controller = basket_controller;
    this->gripper_controller = gripper_controller;
}
