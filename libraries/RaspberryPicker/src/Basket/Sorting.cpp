#include <Arduino.h>
#include "Sorting.h"
const char* sorting_state_to_str(SortingState sorting_state){
    int idx = (int)sorting_state;
    return sorting_state_strings[idx];
};

bool str_to_sorting_state(String sorting_state_str, SortingState* out_sorting_state){
    bool matched = true;
    if (sorting_state_str == "SMALL") {
        *out_sorting_state = SortingState::SMALL;
    } else if (sorting_state_str == "LARGE") {
        *out_sorting_state = SortingState::LARGE;
    } else if (sorting_state_str == "IDLE") {
        *out_sorting_state = SortingState::IDLE;
    }else{
        matched = false;
    }
    return matched;
}
