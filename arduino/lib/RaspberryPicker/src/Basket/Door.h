/**
 * Door.h
 * 
 * Basket door state definitions and serialization utilities.
 * Provides functions to convert between DoorState enum and string representations.
 */

#ifndef RASPBERRY_PICKER_BASKET_DOOR_H
#define RASPBERRY_PICKER_BASKET_DOOR_H

/**
 * BasketDoor class - manages basket door state and configuration.
 * Provides door state serialization and position constants.
 */
class BasketDoor
{
public:
    /**
     * Door State enum:
     * OPEN: The door is open (basket contents can drop out)
     * CLOSED: The door is closed (basket is collecting raspberries)
     */
    enum class DoorState
    {
        OPEN,
        CLOSED,
    };

    /**
     * Converts DoorState enum to string representation.
     */
    static const char *serialize_door_state(BasketDoor::DoorState door_state);
    
    /**
     * Converts string to DoorState enum.
     */
    static bool deserialize_door_state(String door_state_str, BasketDoor::DoorState *out_door_state);

    static const int closed_pos; // Servo position when door is closed [degrees]
    static const int open_pos;   // Servo position when door is open [degrees]
    static const int max_fill;   // Maximum number of raspberries before basket should be emptied
    static const int delay_ms;   // Time to wait for basket to empty after opening door [ms]
};

#endif
