/**
 * Sorting.cpp
 * 
 * Sorting state serialization/deserialization utilities for the basket sorting mechanism.
 * Provides functions to convert between SortingState enum and string representations.
 */

#include <Arduino.h>
#include "Sorting.h"

// String representations of sorting states
static const char *sorting_state_strings[] = {
    "LARGE",
    "SMALL",
    "IDLE"};

/**
 * Converts SortingState enum to string representation.
 * @param sorting_state Sorting state enum value
 * @return String representation of the sorting state
 */
const char *BasketSorter::serialize_sorting_state(BasketSorter::SortingState sorting_state)
{
    int idx = (int)sorting_state;
    return sorting_state_strings[idx];
};

/**
 * Converts string to SortingState enum.
 * @param sorting_state_str String representation of sorting state
 * @param out_sorting_state Pointer to store resulting SortingState enum
 * @return true if string matched a valid state, false otherwise
 */
bool BasketSorter::deserialize_sorting_state(String sorting_state_str, BasketSorter::SortingState *out_sorting_state)
{
    bool matched = true;
    if (sorting_state_str == "SMALL")
    {
        *out_sorting_state = BasketSorter::SortingState::SMALL;
    }
    else if (sorting_state_str == "LARGE")
    {
        *out_sorting_state = BasketSorter::SortingState::LARGE;
    }
    else if (sorting_state_str == "IDLE")
    {
        *out_sorting_state = BasketSorter::SortingState::IDLE;
    }
    else
    {
        matched = false;
    }
    return matched;
}
