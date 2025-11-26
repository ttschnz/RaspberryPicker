#ifndef RASPBERRY_PICKER_CONTROLLER_H
#define RASPBERRY_PICKER_CONTROLLER_H

#include <Arduino.h>

class BasketController;
class GripperController;
class InterfaceMaster;

class Controller
{
public:
    enum State
    {
        MANUAL,
        IDLE,
        PROGRAM,
    };

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

    const char *serialize_program(Program program);
    bool deserialize_program(String program, Program *out_program);
    const char *serialize_state(State state);
    bool deserialize_state(String state, State *out_state);

    Controller(State state);
    Controller(Program program);
    Controller(State state, InterfaceMaster *interface);

    State get_state();
    Program get_program();

    void set_state(State state);
    void set_program(Program program);

    void add_controllers(BasketController *basket_controller, GripperController *gripper_controller);
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
     * Used to open the grabbing mechanism and drop the berry in the correct bsaket
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
     * - resets the counter of the bsket
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

    State state;
    Program program;

    GripperController *gripper_controller;
    BasketController *basket_controller;
    InterfaceMaster *interface;
};

#endif
