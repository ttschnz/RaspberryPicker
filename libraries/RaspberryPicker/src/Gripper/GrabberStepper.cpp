#include <Arduino.h>
#include "GrabberStepper.h"


const char* GrabberStepper::serialize_grabber_state(GrabberStepper::GrabberState grabber_state){
    int idx = (int)grabber_state;
    return grabber_state_strings[idx];
}

bool GrabberStepper::deserialize_grabber_state(String grabber_state_str, GrabberStepper::GrabberState* out_grabber_state){
    bool matched = true;
    if (grabber_state_str=="CLOSED"){
        *out_grabber_state = GrabberStepper::GrabberState::CLOSED;
    }else if (grabber_state_str=="OPEN"){
        *out_grabber_state = GrabberStepper::GrabberState::OPEN;
    }else{
        matched = false;
    }
    return matched;

}


const char* GrabberStepper::serialize_raspberry_size(GrabberStepper::RaspberrySize raspberry_size){
    int idx = (int)raspberry_size;
    return raspberry_size_strings[idx];
}


bool GrabberStepper::deserialize_raspberry_size(String raspberry_size_str, GrabberStepper::RaspberrySize* out_raspberry_size){
    bool matched = true;
    if (raspberry_size_str=="LARGE"){
        *out_raspberry_size = GrabberStepper::RaspberrySize::LARGE;
    }else if (raspberry_size_str=="SMALL"){
        *out_raspberry_size = GrabberStepper::RaspberrySize::SMALL;
    }else{
        matched = false;
    }
    return matched;
}
