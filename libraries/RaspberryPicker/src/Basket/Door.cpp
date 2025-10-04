#include <Arduino.h>
#include "Door.h"

const char* door_state_to_str(DoorState door_state){
    int idx = (int)door_state;
    return door_state_strings[idx];
};

bool str_to_door_state(String door_state_str, DoorState* out_door_state){
    bool matched = true;
    if (door_state_str == "OPEN_SMALL") {
        *out_door_state = DoorState::OPEN_SMALL;
    } else if (door_state_str == "OPEN_LARGE") {
        *out_door_state = DoorState::OPEN_LARGE;
    } else if (door_state_str == "CLOSED") {
        *out_door_state = DoorState::CLOSED;
    }else{
        matched = false;
    }
    return matched;
}
