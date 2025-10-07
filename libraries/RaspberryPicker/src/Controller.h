#ifndef RASPBERRY_PICKER_CONTROLLER_H
#define RASPBERRY_PICKER_CONTROLLER_H

#include <Arduino.h>


class BasketController;
class GripperController;


class Controller{
    public:
        enum State{
            MANUAL,
            IDLE,
            PROGRAM,
        };
        enum Program{
            CLOSE,
            RELEASE,
            DROP,
            RESET,
        };
        Controller(State state);
        Controller(Program program);
        State get_state();
        Program get_program();
        void add_controllers(BasketController* basket_controller, GripperController* gripper_controller);

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

    private:
        State state;
        Program program;

        GripperController* gripper_controller;
        BasketController* basket_controller;
};

#endif
