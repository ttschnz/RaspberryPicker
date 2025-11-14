#ifndef RASPBERRY_PICKER_BASKET_SORTING_H
#define RASPBERRY_PICKER_BASKET_SORTING_H
#include <Arduino.h>

class BasketSorter
{
    public:
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
        static const int large_pos;
        static const int small_pos;
        static const int idle_pos;

        static const char* serialize_sorting_state(BasketSorter::SortingState sorting_state);
        static bool deserialize_sorting_state(String sorting_state_str, BasketSorter::SortingState* out_sorting_state);
};

#endif
