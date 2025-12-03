/**
 * Door.cpp
 * 
 * Door state serialization/deserialization utilities for the basket door.
 * Provides functions to convert between DoorState enum and string representations.
 */

#include <Arduino.h>
#include "Door.h"

// String representations of door states
static const char *door_state_strings[] = {
    "OPEN",
    "CLOSED"};

/**
 * Converts DoorState enum to string representation.
 * @param door_state Door state enum value
 * @return String representation of the door state
 */
const char *BasketDoor::serialize_door_state(BasketDoor::DoorState door_state)
{
    int idx = (int)door_state;
    return door_state_strings[idx];
};

/**
 * Converts string to DoorState enum.
 * @param door_state_str String representation of door state
 * @param out_door_state Pointer to store resulting DoorState enum
 * @return true if string matched a valid state, false otherwise
 */
bool BasketDoor::deserialize_door_state(String door_state_str, BasketDoor::DoorState *out_door_state)
{
    bool matched = true;
    if (door_state_str == "OPEN")
    {
        *out_door_state = BasketDoor::DoorState::OPEN;
    }
    else if (door_state_str == "CLOSED")
    {
        *out_door_state = BasketDoor::DoorState::CLOSED;
    }
    else
    {
        matched = false;
    }
    return matched;
}
