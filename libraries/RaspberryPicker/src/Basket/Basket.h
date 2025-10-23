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

/**
 * Pinout of the Basket
 * each pin represents the digital pin where the servo motors are plugged in
 */
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
        * Initialise the basket controller. Call this during setup.
        */
        BasketController(BasketPinout *pinout, InterfaceMaster *interface);

        /**
         * Gets the angle to be sent to the door servo motor based on the desired state.
         */
        int get_desired_door_pos(BasketDoor::DoorState desired_door_state);

        /**
         * opens or closes the basket's door synchronously (wait until done).
         */
        void set_door(BasketDoor::DoorState target_state);

        /**
         * resets the counter for the currently open door.
         * returns false if no door is open
         */
        bool reset_counter(bool force);

        /**
         * * Gets the angle to be sent to the sorting servo motor based on the desired state.
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
        BasketDoor::DoorState door_state;
        InterfaceMaster *interface;
} ;
#endif
