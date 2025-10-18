#include <Arduino.h>
#include "GrabberStepper.h"


const char* grabber_state_to_str(GrabberState grabber_state){
    int idx = (int)grabber_state;
    return grabber_state_strings[idx];
}

bool str_to_grabber_state(String grabber_state_str, GrabberState* out_grabber_state){
    bool matched = true;
    if (grabber_state_str=="CLOSED"){
        *out_grabber_state = GrabberState::CLOSED;
    }else if (grabber_state_str=="OPEN"){
        *out_grabber_state = GrabberState::OPEN;
    }else{
        matched = false;
    }
    return matched;

}
