#include <Arduino.h>
#include "GripperStepper.h"


const char* GripperStepper::serialize_gripper_state(GripperStepper::GripperState gripper_state){
    int idx = (int)gripper_state;
    return gripper_state_strings[idx];
}

bool GripperStepper::deserialize_gripper_state(String gripper_state_str, GripperStepper::GripperState* out_gripper_state){
    bool matched = true;
    if (gripper_state_str=="CLOSED"){
        *out_gripper_state = GripperStepper::GripperState::CLOSED;
    }else if (gripper_state_str=="OPEN"){
        *out_gripper_state = GripperStepper::GripperState::OPEN;
    }else{
        matched = false;
    }
    return matched;

}


const char* GripperStepper::serialize_raspberry_size(GripperStepper::RaspberrySize raspberry_size){
    int idx = (int)raspberry_size;
    return raspberry_size_strings[idx];
}


bool GripperStepper::deserialize_raspberry_size(String raspberry_size_str, GripperStepper::RaspberrySize* out_raspberry_size){
    bool matched = true;
    if (raspberry_size_str=="LARGE"){
        *out_raspberry_size = GripperStepper::RaspberrySize::LARGE;
    }else if (raspberry_size_str=="SMALL"){
        *out_raspberry_size = GripperStepper::RaspberrySize::SMALL;
    }else{
        matched = false;
    }
    return matched;
}
