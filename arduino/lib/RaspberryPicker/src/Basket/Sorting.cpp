#include <Arduino.h>
#include "Sorting.h"

static const char *sorting_state_strings[] = {
    "LARGE",
    "SMALL",
    "IDLE"};

const char *BasketSorter::serialize_sorting_state(BasketSorter::SortingState sorting_state)
{
    int idx = (int)sorting_state;
    return sorting_state_strings[idx];
};

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
