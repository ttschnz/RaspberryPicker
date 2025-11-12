#include <Arduino.h>
#include "GripperStepper.h"


const char* GripperStepper::serialize_gripper_state(GripperStepper::GripperState gripper_state){
    int idx = (int)gripper_state;
    return gripper_state_strings[idx];
}

bool GripperStepper::deserialize_gripper_state(String gripper_state_str, GripperStepper::GripperState* out_gripper_state){
    bool matched = true;
    if (gripper_state_str=="CLOSED"){
        *out_gripper_state = GripperStepper::GripperState::CLOSED_SMALL;
    }else if (gripper_state_str=="OPEN"){
        *out_gripper_state = GripperStepper::GripperState::OPEN;
    }else{
        matched = false;
    }
    return matched;

}

int GripperStepper::mm_to_steps(int mm){
    double steps =
        (double)mm   // [mm]
        * (double)GripperStepper::steps_per_revolution // [stp/rot]
        / (double)GripperStepper::transmission_ratio; // [mm/rot]
    return (int)steps;
}


int GripperStepper::steps_to_mm(int steps){
    double mm =
        (double)steps   // [mm]
        * (double)GripperStepper::transmission_ratio // [mm/rot]
        / (double)GripperStepper::steps_per_revolution; // [stp/rot]
    return (int)mm;
}

int GripperStepper::get_desired_step_position(GripperState state){
    int desired_mm = 0;
    switch(state){
        case GripperStepper::GripperState::OPEN:
            desired_mm = GripperStepper::plate_distance_open;
            break;
        case GripperStepper::GripperState::CLOSED_SMALL:
            desired_mm = GripperStepper::plate_distance_small;
            break;
        case GripperStepper::GripperState::CLOSED_LARGE:
            desired_mm = GripperStepper::plate_distance_large;
            break;
    }

    int desired_steps = GripperStepper::mm_to_steps(desired_mm);

    return desired_steps;
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
