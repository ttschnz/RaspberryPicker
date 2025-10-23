#include "Controller.h"

#include "Basket/Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

#include "Gripper/Gripper.h"
#include "Gripper/GrabberStepper.h"
Controller::Controller(State state){
    Controller(state, nullptr);
}
Controller::Controller(State state, InterfaceMaster *interface){
    if (state == State::PROGRAM){
        state = State::IDLE; // default to idle
    }
    this->basket_controller = nullptr;
    this->gripper_controller = nullptr;
    this->interface = interface;
    this->set_state(state);
}

Controller::Controller(Program program){
    this->set_program(program);
}

Controller::Program Controller::get_program(){
    return this->program;
}

Controller::State Controller::get_state(){
    return this->state;
}

void Controller::set_program(Controller::Program program){
    this->set_state(Controller::State::PROGRAM);
    this->program = program;
    if(this->interface != nullptr){
        this->interface->send_state("controller.program", this->serialize_program(this->get_program()));
    }
}

void Controller::set_state(Controller::State state){
    this->state = state;
    if(this->interface != nullptr){
        this->interface->send_state("controller.state", this->serialize_state(this->get_state()));
    }
}

void Controller::add_controllers(BasketController* basket_controller, GripperController* gripper_controller){
    this->basket_controller = basket_controller;
    this->gripper_controller = gripper_controller;
}

void Controller::add_interface(InterfaceMaster* interface){
    this->interface = interface;
}

void Controller::run_close(){
    // color sensor
    bool is_ripe = this->gripper_controller->is_ripe();
    this->interface->send_state("gripper.raspberry_ripeness", is_ripe ? "RIPE" : "UNRIPE");

    if (!is_ripe){
        return;
    }

    // close grabbing mechanism
    RaspberrySize size = this->gripper_controller->set_grabber(GrabberState::CLOSED);
    this->interface->send_state("gripper.raspberry_size", raspberry_size_to_str(size));


    // set sorting to the correct position
    switch(size){
        case RaspberrySize::LARGE:
            this->basket_controller->set_sorting(SortingState::LARGE);
            break;
        case RaspberrySize::SMALL:
            this->basket_controller->set_sorting(SortingState::SMALL);
            break;
    }
}

void Controller::run_release(){
    this->gripper_controller->set_grabber(GrabberState::OPEN);
    if (this->basket_controller->increment_counter()==false){
        Serial.println((String)"cannot increment counter on sorting state " + sorting_state_to_str(this->basket_controller->sorting_state));
    }
}

void Controller::run_drop(){
    FillCount fill_count = this->basket_controller->fill_count;
    if (fill_count.fill_large >= BasketDoor::max_fill){
        this->basket_controller->set_door(BasketDoor::DoorState::OPEN_LARGE);
        this->basket_controller->reset_counter(false);
        this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
    }
    if (fill_count.fill_small >= BasketDoor::max_fill){
        this->basket_controller->set_door(BasketDoor::DoorState::OPEN_SMALL);
        this->basket_controller->reset_counter(false);
        this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
    }
}

void Controller::run_reset(){
    this->gripper_controller->set_grabber(GrabberState::OPEN);
    this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
    this->basket_controller->set_sorting(SortingState::IDLE);
    this->basket_controller->reset_counter(true);
}

void Controller::run_calibrate_color(){
    Serial.println("calibrating colors");
    this->gripper_controller->color_sensor->calibrate();
}


const char* Controller::serialize_program(Program program){
    int idx = static_cast<int>(program);

    const char * program_strings[5] ={
        "CLOSE",
        "RELEASE",
        "DROP",
        "RESET",
        "CALIBRATE_COLOR"
    };
    return program_strings[idx];
}
bool Controller::deserialize_program(String program, Controller::Program* out_program){
    bool matched = true;
    if (program == "CLOSE"){
        *out_program = Controller::Program::CLOSE;
    } else if(program== "RELEASE"){
        *out_program = Controller::Program::RELEASE;
    } else if(program== "DROP"){
        *out_program = Controller::Program::DROP;
    } else if(program== "RESET"){
        *out_program = Controller::Program::RESET;
    } else if(program == "CALIBRATE_COLOR"){
        *out_program = Controller::Program::CALIBRATE_COLOR;
    } else{
        matched = false;
    }
    return matched;
}

const char* Controller::serialize_state(State state){
    int idx = static_cast<int>(state);
    const char* state_strings[3] = {
        "MANUAL",
        "IDLE",
        "PROGRAM"
    };
    return state_strings[idx];
}

bool Controller::deserialize_state(String state, Controller::State* out_state){
    bool matched = true;
    if (state == "MANUAL"){
        *out_state = Controller::State::MANUAL;
    } else if(state== "IDLE"){
        *out_state = Controller::State::IDLE;
    } else if(state== "PROGRAM"){
        *out_state = Controller::State::PROGRAM;
    } else {
        matched = false;
    }
    return matched;
}
