/**
 * Sorting.h
 * 
 * Basket sorting mechanism state definitions and serialization utilities.
 * Provides functions to convert between SortingState enum and string representations.
 */

#ifndef RASPBERRY_PICKER_BASKET_SORTING_H
#define RASPBERRY_PICKER_BASKET_SORTING_H
#include <Arduino.h>

/**
 * BasketSorter class - manages sorting mechanism state and configuration.
 * Controls the servo that directs raspberries to small or large compartments.
 */
class BasketSorter
{
public:
    /**
     * Sorting State enum:
     * LARGE: Sorting mechanism directs raspberries to large compartment
     * SMALL: Sorting mechanism directs raspberries to small compartment
     * IDLE: Sorting mechanism is in neutral/center position
     */
    enum class SortingState
    {
        LARGE,
        SMALL,
        IDLE,
    };
    
    static const int large_pos;  // Servo position for directing to large compartment [degrees]
    static const int small_pos;  // Servo position for directing to small compartment [degrees]
    static const int idle_pos;   // Servo position for neutral/idle state [degrees]

    /**
     * Converts SortingState enum to string representation.
     */
    static const char *serialize_sorting_state(BasketSorter::SortingState sorting_state);
    
    /**
     * Converts string to SortingState enum.
     */
    static bool deserialize_sorting_state(String sorting_state_str, BasketSorter::SortingState *out_sorting_state);
};

#endif
