#ifndef RASPBERRY_PICKER_BASKET_SORTING_H
#define RASPBERRY_PICKER_BASKET_SORTING_H
#include <Arduino.h>
/**
 * Sorting State:
 * - LARGE: The switcher is switched for large berries
 * - SMALL: The switcher is switched for small berries
 * - IDLE: The switcher is in the center and ready to switch in a direction
 */
enum class SortingState {
    LARGE,
    SMALL,
    IDLE,
} ;
static const char* sorting_state_strings[] = {
    "LARGE",
    "SMALL",
    "IDLE"
};

const char* sorting_state_to_str(SortingState sorting_state);
bool str_to_sorting_state(String sorting_state_str, SortingState* out_sorting_state);

class SortingServoValues
{
    public:
        static const int large_pos;
        static const int small_pos;
        static const int idle_pos;
        static const int speed;
};

#endif
