#include "Controller.h"
#include "Basket.h"
#include "Gripper.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"
#include "Gripper/GrabberStepper.h"

Controller::Controller(State state){
    if (state == State::PROGRAM){
        state = State::IDLE; // default to idle
    }
    this->state = state;
}

Controller::Controller(Program program){
    this->state = State::PROGRAM;
    this->program = program;
}

Controller::Program Controller::get_program(){
    return this->program;
}

Controller::State Controller::get_state(){
    return this->state;
}

void Controller::add_controllers(BasketController* basket_controller, GripperController* gripper_controller){
    this->basket_controller = basket_controller;
    this->gripper_controller = gripper_controller;
}

void Controller::run_close(){
    // color sensor
    bool is_ripe = this->gripper_controller->measure_color();

    if (!is_ripe){
        return;
    }

    // close grabbing mechanism
    RaspberrySize size = this->gripper_controller->set_grabber(GrabberState::CLOSED);

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
