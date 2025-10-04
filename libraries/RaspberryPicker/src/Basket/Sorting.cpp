#include "Sorting.h"

const char* sorting_state_to_str(SortingState sorting_state){
    int idx = (int)sorting_state;
    return sorting_state_strings[idx];
};
