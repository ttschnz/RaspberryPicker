/**
 * Basket.h
 * 
 * Basket controller for the Raspberry Picker.
 * Manages the basket door servo (for emptying) and sorting servo (for size separation).
 * Tracks fill counts for small and large raspberry compartments.
 */

#ifndef RASPBERRY_PICKER_BASKET_H
#define RASPBERRY_PICKER_BASKET_H

#include <Arduino.h>
#include <Servo.h>

#include "../InterfaceMaster.h"

#include "Door.h"
#include "Sorting.h"

/**
 * FillCount structure - tracks raspberry count in each compartment.
 * fill_small: Number of small raspberries collected
 * fill_large: Number of large raspberries collected
 */
typedef struct
{
    int fill_small;
    int fill_large;
} FillCount;

/**
 * BasketPinout structure - defines servo motor pin assignments.
 * sorting_pin: Digital pin for sorting servo motor
 * door_pin: Digital pin for door servo motor
 */
typedef struct
{
    int sorting_pin;
    int door_pin;
} BasketPinout;

/**
 * BasketController class - controls basket door and sorting mechanism.
 * Manages servo motors and tracks fill counts for each compartment.
 */
class BasketController
{
public:
    /**
     * Constructor - initializes basket controller with pin configuration.
     * Call this during setup.
     */
    BasketController(BasketPinout *pinout, InterfaceMaster *interface);

    /**
     * Gets the servo angle for the specified door state.
     */
    int get_desired_door_pos(BasketDoor::DoorState desired_door_state);

    /**
     * Opens or closes the basket's door synchronously (waits until complete).
     */
    void set_door(BasketDoor::DoorState target_state);

    /**
     * Resets the counter for the currently open door.
     * @param force If true, resets regardless of door state
     * @return false if no door is open and force is false
     */
    bool reset_counter(bool force);

    /**
     * Gets the servo angle for the specified sorting state.
     */
    int get_desired_sorting_pos(BasketSorter::SortingState desired_sorting_state);

    /**
     * Sets the sorting mechanism to a specified position.
     */
    void set_sorting(BasketSorter::SortingState target_state);

    /**
     * Increments the counter by one for the current sorting state.
     * @return false if sorting is in IDLE state
     */
    bool increment_counter();

    FillCount fill_count;                          // Current fill counts for both compartments
    BasketSorter::SortingState sorting_state;      // Current sorting mechanism state

private:
    Servo sorting_servo;                           // Servo for sorting mechanism
    Servo door_servo;                              // Servo for basket door
    int sorting_pos;                               // Current sorting servo position
    int door_pos;                                  // Current door servo position
    BasketDoor::DoorState door_state;              // Current door state
    InterfaceMaster *interface;                    // Pointer to interface master
};
#endif
