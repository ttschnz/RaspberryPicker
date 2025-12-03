/**
 * InterfaceMaster.h
 * 
 * Serial communication interface for the Raspberry Picker.
 * Handles bidirectional communication:
 * - Receives commands from external control systems via serial
 * - Sends state updates and sensor data back via serial
 * - Parses key=value pairs for state changes
 * - Routes commands to appropriate controllers
 */

#ifndef RASPBERRY_PICKER_INTERFACE_MASTER_H
#define RASPBERRY_PICKER_INTERFACE_MASTER_H

#include <SoftwareSerial.h>
#include <Arduino.h>

#include "Controller.h"

class BasketController;
class GripperController;

/**
 * InterfaceMaster class - manages serial communication with external systems.
 * Receives commands and sends state updates via serial interface.
 */
class InterfaceMaster
{
public:
    /**
     * Constructor - initializes interface with null controller references.
     */
    InterfaceMaster();
    
    /**
     * Template method to send state updates via serial interface.
     * Formats data as key=value pairs for transmission.
     * @param key State variable identifier
     * @param value Current value of the state variable
     */
    template <typename T>
    void send_state(const char *key, T value)
    {
        String str_value = String(value); // Convert value to String internally
        String data_buffer;
        data_buffer.concat(key);
        data_buffer.concat("=");
        data_buffer.concat(str_value);
        Serial.println(data_buffer);
        // this->controller = new Controller(Controller::State::IDLE);
    };

    /**
     * Listens for and processes state change requests from serial interface.
     * Expects commands in format: key=value
     * Routes commands to appropriate controllers.
     */
    void listen_state_change_requests();
    
    /**
     * Adds references to basket and gripper controllers.
     * Must be called during initialization to enable command routing.
     */
    void add_controllers(BasketController *basket_controller, GripperController *gripper_controller);
    
    Controller *controller;  // Pointer to main controller

private:
    // SoftwareSerial* Serial;
    BasketController *basket_controller;      // Pointer to basket controller
    GripperController *gripper_controller;    // Pointer to gripper controller
};

#endif
