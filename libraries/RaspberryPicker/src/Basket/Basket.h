#ifndef RASPBERRY_PICKER_BASKET_H
#define RASPBERRY_PICKER_BASKET_H

#include <Arduino.h>
#include <Servo.h>

#include "../InterfaceMaster.h"

#include "Door.h"
#include "Sorting.h"

/**
 * FillCount:
 * counts how many berries are in each basket
 */
typedef struct {
    int fill_small;
    int fill_large;
} FillCount;

typedef struct {
    int sorting_pin;
    int door_pin;
} BasketPinout;

/**
 * Controller for the basket
 */
class BasketController {
    public:
        /**
        * Initialise the basket controller. Call during setup.
        */
        BasketController(BasketPinout *pinout, InterfaceMaster *interface);

        /**
         * Reads the opening state from the position and last command
         */
        int get_desired_door_pos(DoorState desired_door_state);

        /**
         * opens the basket if it is ready. returns false if the door is not ready to be closed.
         * door: 0 for small, 1 for large.
         */
        void set_door(DoorState target_state);

        /**
         * resets the counter for the currently open door. returns false if no door is open
         */
        bool reset_counter(bool force);

        /**
         * returns the position to whchi the servo has to move in order to reach a certain state
         */
        int get_desired_sorting_pos(SortingState desired_sorting_state);

        /**
         * Sets the sorting to a certain position
         */
        void set_sorting(SortingState target_state);

        /**
         * increments the counter by one for the current sorting state. returns false if we are idle
         */
        bool increment_counter();

        FillCount fill_count;
        SortingState sorting_state;
    private:
        Servo sorting_servo;
        Servo door_servo;
        int sorting_pos;
        int door_pos;
        DoorState door_state;
        InterfaceMaster *interface;
} ;
#endif
