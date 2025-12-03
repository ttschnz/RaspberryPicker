/**
 * Basket.cpp
 * 
 * Implementation of the basket controller for the Raspberry Picker.
 * Manages the basket door servo (for emptying) and sorting servo (for size separation).
 * Tracks fill counts for small and large raspberry compartments.
 */

#include <Arduino.h>
#include <Servo.h>

#include "../InterfaceMaster.h"

#include "Basket.h"
#include "Door.h"
#include "Sorting.h"

// Sorting servo position constants (in degrees)
const int BasketSorter::small_pos = 20;   // Position to direct raspberries to small compartment
const int BasketSorter::idle_pos = 86;    // Neutral/idle position
const int BasketSorter::large_pos = 152;  // Position to direct raspberries to large compartment

// Door servo position constants (in degrees)
const int BasketDoor::closed_pos = 170;  // Position when door is closed
const int BasketDoor::open_pos = 10;     // Position when door is open
const int BasketDoor::max_fill = 23;     // Maximum fill count before basket should be emptied
const int BasketDoor::delay_ms = 10000;  // Time to wait for basket to empty (10 seconds)

/**
 * Constructor - initializes basket controller with pin configuration.
 * @param pinout Pointer to BasketPinout structure with pin assignments
 * @param interface Pointer to InterfaceMaster for state communication
 */
BasketController::BasketController(BasketPinout *pinout, InterfaceMaster *interface)
{
    this->interface = interface;

    // Initialize fill count values to zero
    fill_count.fill_small = 0;
    fill_count.fill_large = 0;

    // Attach the servos using the pins from the BasketPinout struct
    door_servo.attach(pinout->door_pin);
    sorting_servo.attach(pinout->sorting_pin);

    pinMode(pinout->door_pin, OUTPUT);
    pinMode(pinout->sorting_pin, OUTPUT);

    // Initialize servos to default positions
    this->set_sorting(BasketSorter::SortingState::IDLE);
    this->set_door(BasketDoor::DoorState::CLOSED);
}

/**
 * Gets the desired servo position for a given door state.
 * @param new_door_state Desired door state
 * @return Servo position in degrees
 */
int BasketController::get_desired_door_pos(BasketDoor::DoorState new_door_state)
{
    int desired_pos;

    switch (new_door_state)
    {
    case BasketDoor::DoorState::CLOSED:
        desired_pos = BasketDoor::closed_pos;
        break;
    case BasketDoor::DoorState::OPEN:
        desired_pos = BasketDoor::open_pos;
        break;
    };
    Serial.println((String) "desired door pos: " + desired_pos);
    return desired_pos;
}

/**
 * Sets the basket door to the specified state.
 * @param target_state Desired door state (OPEN or CLOSED)
 */
void BasketController::set_door(BasketDoor::DoorState target_state)
{
    int target_position = this->get_desired_door_pos(target_state);
    this->door_state = target_state;
    this->door_servo.write(target_position);
    this->interface->send_state("basket.door.state", BasketDoor::serialize_door_state(target_state));
    this->interface->send_state("basket.door.position", target_position);
}

/**
 * Resets the fill counters for both compartments.
 * @param force If true, resets regardless of door state. If false, only resets when door is open.
 * @return true if counters were reset, false otherwise
 */
bool BasketController::reset_counter(bool force)
{
    bool reset = (this->door_state == BasketDoor::DoorState::OPEN) || force;

    if (reset)
    {
        this->fill_count.fill_small = 0;
        this->interface->send_state("basket.fill_count.small", this->fill_count.fill_small);
        this->fill_count.fill_large = 0;
        this->interface->send_state("basket.fill_count.large", this->fill_count.fill_large);
    }

    return reset;
}

/**
 * Gets the desired servo position for a given sorting state.
 * @param new_sorting_state Desired sorting state
 * @return Servo position in degrees
 */
int BasketController::get_desired_sorting_pos(BasketSorter::SortingState new_sorting_state)
{
    int desired_pos;

    switch (new_sorting_state)
    {
    case BasketSorter::SortingState::IDLE:
        desired_pos = BasketSorter::idle_pos;
        break;
    case BasketSorter::SortingState::SMALL:
        desired_pos = BasketSorter::small_pos;
        break;
    case BasketSorter::SortingState::LARGE:
        desired_pos = BasketSorter::large_pos;
        break;
    };
    return desired_pos;
}

/**
 * Sets the sorting mechanism to the specified state.
 * @param target_state Desired sorting state (IDLE, SMALL, or LARGE)
 */
void BasketController::set_sorting(BasketSorter::SortingState target_state)
{
    int target_position = this->get_desired_sorting_pos(target_state);
    this->sorting_state = target_state;
    this->interface->send_state("basket.sorting.state", BasketSorter::serialize_sorting_state(target_state));
    this->sorting_servo.write(target_position);
    this->interface->send_state("basket.sorting.position", target_position);
}

/**
 * Increments the fill counter for the currently active compartment.
 * @return true if counter was incremented, false if sorting is in IDLE state
 */
bool BasketController::increment_counter()
{
    switch (this->sorting_state)
    {
    case BasketSorter::SortingState::IDLE:
        // Cannot increment when not actively sorting
        return false;
        break;
    case BasketSorter::SortingState::SMALL:
        this->fill_count.fill_small += 1;
        this->interface->send_state("basket.fill_count.small", this->fill_count.fill_small);
        break;
    case BasketSorter::SortingState::LARGE:
        this->fill_count.fill_large += 1;
        this->interface->send_state("basket.fill_count.large", this->fill_count.fill_large);
        break;
    }
    return true;
}
