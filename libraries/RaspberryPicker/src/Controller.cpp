#include "Controller.h"

#include "Basket/Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

#include "Gripper/Gripper.h"
#include "Gripper/GrabberStepper.h"

Controller::Controller(State state){
    if (state == State::PROGRAM){
        state = State::IDLE; // default to idle
    }
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
    this->interface->send_state("controller.program", Controller::serialize_program(program));
}

void Controller::set_state(Controller::State state){
    this->state = state;
    this->interface->send_state("controller.state", Controller::serialize_state(state));
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
    bool is_ripe = this->gripper_controller->measure_color();

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

    // ready for pull down
    // TODO: How do we indicate readyness?
}

void Controller::run_release(){
    this->gripper_controller->set_grabber(GrabberState::OPEN);
    this->basket_controller->increment_counter(); // TODO: handle return value
}

void Controller::run_drop(){
    FillCount fill_count = this->basket_controller->fill_count;
    if (fill_count.fill_large >= DoorValues::max_fill){
        this->basket_controller->set_door(DoorState::OPEN_LARGE);
        this->basket_controller->reset_counter();
        this->basket_controller->set_door(DoorState::CLOSED);
    }
    if (fill_count.fill_small >= DoorValues::max_fill){
        this->basket_controller->set_door(DoorState::OPEN_SMALL);
        this->basket_controller->reset_counter();
        this->basket_controller->set_door(DoorState::CLOSED);
    }
}

void Controller::run_reset(){
    this->gripper_controller->set_grabber(GrabberState::OPEN);
    this->basket_controller->set_door(DoorState::CLOSED);
    this->basket_controller->set_sorting(SortingState::IDLE);
}

const char* Controller::serialize_program(Program program){
    int idx = (int)program;
    return Controller::program_strings[idx];
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
    } else {
        matched = false;
    }
    return matched;
}

const char* Controller::serialize_state(State state){
    int idx = (int)program;
    return Controller::state_strings[idx];
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
