#include <Arduino.h>
#include "Door.h"

const char* BasketDoor::serialize_door_state(BasketDoor::DoorState door_state){
    int idx = (int)door_state;
    return door_state_strings[idx];
};

bool BasketDoor::deserialize_door_state(String door_state_str, BasketDoor::DoorState* out_door_state){
    bool matched = true;
    if (door_state_str == "OPEN_SMALL") {
        *out_door_state = BasketDoor::DoorState::OPEN_SMALL;
    } else if (door_state_str == "OPEN_LARGE") {
        *out_door_state = BasketDoor::DoorState::OPEN_LARGE;
    } else if (door_state_str == "CLOSED") {
        *out_door_state = BasketDoor::DoorState::CLOSED;
    }else{
        matched = false;
    }
    return matched;
}
