/**
 * Controller.h
 * 
 * Main controller for the Raspberry Picker robot.
 * Manages the overall state machine and coordinates basket and gripper operations.
 * Implements various programs for automated raspberry picking, sorting, and basket management.
 */

#ifndef RASPBERRY_PICKER_CONTROLLER_H
#define RASPBERRY_PICKER_CONTROLLER_H

#include <Arduino.h>

class BasketController;
class GripperController;
class InterfaceMaster;

/**
 * Controller class - manages the overall system state and coordinates operations.
 * Provides state machine control and automated program execution.
 */
class Controller
{
public:
    /**
     * State enum - defines operating modes for the controller.
     * MANUAL: Direct control of individual components
     * IDLE: System is idle, waiting for commands
     * PROGRAM: Executing an automated program
     */
    enum State
    {
        MANUAL,
        IDLE,
        PROGRAM,
    };

    /**
     * Program enum - defines automated sequences.
     * CLOSE_GRIPPER: Close gripper, sense color, and position sorter
     * RELEASE_GRIPPER: Open gripper and increment counter
     * EMPTY_BASKET: Open basket door and reset counters
     * RESET: Reset all components to initial positions
     * MEASURE_COLOR: Continuous color measurement for calibration
     * PROGRAM_1: Full automated picking cycle
     * PROGRAM_2: Empty basket program
     */
    enum Program
    {
        CLOSE_GRIPPER,
        RELEASE_GRIPPER,
        EMPTY_BASKET,
        RESET,
        MEASURE_COLOR,
        PROGRAM_1,
        PROGRAM_2,
    };

    /**
     * Converts program enum to string representation.
     */
    const char *serialize_program(Program program);
    
    /**
     * Converts string to program enum.
     */
    bool deserialize_program(String program, Program *out_program);
    
    /**
     * Converts state enum to string representation.
     */
    const char *serialize_state(State state);
    
    /**
     * Converts string to state enum.
     */
    bool deserialize_state(String state, State *out_state);

    /**
     * Constructor - creates controller with specified initial state.
     */
    Controller(State state);
    
    /**
     * Constructor - creates controller with a specific program to execute.
     */
    Controller(Program program);
    
    /**
     * Constructor - creates controller with state and interface.
     */
    Controller(State state, InterfaceMaster *interface);

    /**
     * Gets the current controller state.
     */
    State get_state();
    
    /**
     * Gets the currently selected program.
     */
    Program get_program();

    /**
     * Sets the controller state.
     */
    void set_state(State state);
    
    /**
     * Sets the program to be executed and transitions to PROGRAM state.
     */
    void set_program(Program program);

    /**
     * Adds references to basket and gripper controllers for coordinated operation.
     */
    void add_controllers(BasketController *basket_controller, GripperController *gripper_controller);
    
    /**
     * Adds reference to interface master for communication.
     */
    void add_interface(InterfaceMaster *interface);
    /**
     * Program CLOSE:
     * Used to close the grabbing mechanism
     * - senses the color of the raspberry
     * - proceeds (if ripe) with closing until both sides touch
     * - sets sorting to the correct position
     * - turns on indicator as soon as ready for pull down
     */
    void run_close();
    /**
     * Program RELEASE:
     * Used to open the grabbing mechanism and drop the berry in the correct basket
     * Assumes that the sorting mechanism is already in place
     * - opens the grabbing mechanism
     * - increments counter of basket by one
     */
    void run_release();
    /**
     * Program DROP:
     * Used to drop contents of full basket into the final baskets
     * - finds out which basket is full
     * - opens the door of said basket
     * - resets the counter of the basket
     * - (delay)
     * - closes the door
     * - does the same if the other basket is also full
     */
    void run_drop();
    /**
     * Program RESET:
     * Used to reset the system
     * - sets sorter to central position (IDLE)
     * - moves grabbing mechanism to the open state
     * - closes the doors of the basket (CLOSED)
     */
    void run_reset();

    /**
     * Program MEASURE_COLOR:
     * Used to test the color sensor and find okay threshold values
     */
    void run_measure_color();


    /**
     * Program 1:
     * Close gripper and choose to pick or not. wait until picked and then reset.
     */
    void run_pgm1();


    /**
     * Program 2:
     * Empty the basket
     */
    void run_pgm2();

    State state;                             // Current controller state
    Program program;                         // Currently selected program

    GripperController *gripper_controller;   // Pointer to gripper controller
    BasketController *basket_controller;     // Pointer to basket controller
    InterfaceMaster *interface;              // Pointer to interface master
};

#endif
