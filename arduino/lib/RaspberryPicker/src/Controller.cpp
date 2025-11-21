#include "Controller.h"

#include "Basket/Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

#include "Gripper/Gripper.h"
#include "Gripper/GripperStepper.h"
#include "Gripper/PressureSensor.h"
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
    GripperStepper::RaspberrySize size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LARGE);
    
    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    if (size == GripperStepper::RaspberrySize::UNKNOWN){
        size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_SMALL);
    }
    
    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));

    if (size == GripperStepper::RaspberrySize::UNKNOWN){
        size = this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LIMIT);
    }

    this->interface->send_state("gripper.raspberry_size", GripperStepper::serialize_raspberry_size(size));


    // set sorting to the correct position
    switch(size){
        case GripperStepper::RaspberrySize::LARGE:
            this->basket_controller->set_sorting(BasketSorter::SortingState::LARGE);
            break;
        case GripperStepper::RaspberrySize::SMALL:
            this->basket_controller->set_sorting(BasketSorter::SortingState::SMALL);
            break;
        case GripperStepper::RaspberrySize::UNKNOWN:
            // we have closed the gripper until the limit switch and not felt any touch. abort and re-open.
            this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
            break;
    }
}

void Controller::run_release(){
    this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
    if (this->basket_controller->increment_counter()==false){
        Serial.println((String)"cannot increment counter on sorting state " + BasketSorter::serialize_sorting_state(this->basket_controller->sorting_state));
    }
    this->basket_controller->set_sorting(BasketSorter::SortingState::IDLE);
}

void Controller::run_drop(){
    this->basket_controller->set_door(BasketDoor::DoorState::OPEN);
    this->basket_controller->reset_counter(false);
    delay(BasketDoor::delay_ms);
    this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
}

void Controller::run_reset(){
    this->gripper_controller->set_gripper(GripperStepper::GripperState::CLOSED_LIMIT);
    this->gripper_controller->set_gripper(GripperStepper::GripperState::OPEN);
    this->basket_controller->set_door(BasketDoor::DoorState::CLOSED);
    this->basket_controller->set_sorting(BasketSorter::SortingState::IDLE);
    this->basket_controller->reset_counter(true);
}

void Controller::run_calibrate_color(){
    Serial.println("calibrating colors");
    this->gripper_controller->color_sensor->calibrate();
}

void Controller::run_measure_pressure(){
    while (Serial.available()==0){
        this->gripper_controller->pressure_sensor->is_touching(true);
        delay(100);
    }
}

void Controller::run_measure_color(){
    while (Serial.available()==0){
        RGB rgb_raw;
        this->gripper_controller->color_sensor->measure_rgb_raw(&rgb_raw.r,&rgb_raw.g,&rgb_raw.b);
        //RGB rgb_measured = this->gripper_controller->color_sensor->measure_rgb();
        Serial.println((String)"raw_value:" + rgb_raw.r + "/" + rgb_raw.g + "/" + rgb_raw.b);
        //Serial.println((String)"normalised_value:" + rgb_measured.r + "/" + rgb_measured.g + "/" + rgb_measured.b);
    
        delay(100);
    }
}

const char* Controller::serialize_program(Program program){
    int idx = static_cast<int>(program);

    const char * program_strings[] ={
        "CLOSE_GRIPPER",
        "RELEASE_GRIPPER",
        "EMPTY_BASKET",
        "RESET",
        "CALIBRATE_COLOR",
        "MEASURE_PRESSURE",
        "MEASURE_COLOR",
    };
    return program_strings[idx];
}
bool Controller::deserialize_program(String program, Controller::Program* out_program){
    bool matched = true;
    if (program == "CLOSE_GRIPPER"){
        *out_program = Controller::Program::CLOSE_GRIPPER;
    } else if(program== "RELEASE_GRIPPER"){
        *out_program = Controller::Program::RELEASE_GRIPPER;
    } else if(program== "EMPTY_BASKET"){
        *out_program = Controller::Program::EMPTY_BASKET;
    } else if(program== "RESET"){
        *out_program = Controller::Program::RESET;
    } else if(program == "CALIBRATE_COLOR"){
        *out_program = Controller::Program::CALIBRATE_COLOR;
    } else if(program == "MEASURE_PRESSURE"){
        *out_program = Controller::Program::MEASURE_PRESSURE;
    } else if(program == "MEASURE_COLOR"){
        *out_program = Controller::Program::MEASURE_COLOR;
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
