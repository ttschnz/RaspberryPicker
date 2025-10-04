#include "Door.h"

const char* door_state_to_str(DoorState door_state){
    int idx = (int)door_state;
    return door_state_strings[idx];
};
