#include <Arduino.h>
#include <Servo.h>
#include "Basket.h"
#include "Basket/Door.h"
#include "Basket/Sorting.h"

const int SortingServoValues::small_pos = 0;
const int SortingServoValues::idle_pos = 90;
const int SortingServoValues::large_pos = 180;
const int SortingServoValues::speed = 500;

const int DoorValues::open_small_pos = 0;
const int DoorValues::closed_pos = 90;
const int DoorValues::open_large_pos = 180;
const int DoorValues::speed = 300;
const int DoorValues::max_fill = 23;

BasketController::BasketController(BasketPinout *pinout,InterfaceSender *interface){
    this->interface = interface;

    // Initialize fill count values to zero
    fill_count.fill_small = 0;
    fill_count.fill_large = 0;

    // Attach the servos using the pins from the BasketPinout struct
    door_servo.attach(pinout->door_pin);
    sorting_servo.attach(pinout->sorting_pin);

    pinMode(pinout->door_pin, OUTPUT);
    pinMode(pinout->sorting_pin, OUTPUT);

    // Initialize position variables
    this->set_sorting(SortingState::IDLE);
    this->set_door(DoorState::CLOSED);
}

int BasketController::get_desired_door_pos(DoorState new_door_state){
    int desired_pos;

    switch (this->door_state){
        case DoorState::CLOSED:
            desired_pos = DoorValues::closed_pos;
            break;
        case DoorState::OPEN_SMALL:
            desired_pos = DoorValues::open_small_pos;
            break;
        case DoorState::OPEN_LARGE:
            desired_pos = DoorValues::open_large_pos;
            break;
    };
    return desired_pos;
}

void BasketController::set_door(DoorState target_state){
    int target_position = this->get_desired_door_pos(target_state);
    int angular_distance = abs(this->door_servo.read() - target_position);  // angular distance in degrees
    int time_to_reach = (angular_distance * 1000) / DoorValues::speed; // [deg]/[deg/s]*[1000ms/s]=[ms]
    this->door_state = target_state;
    this->door_servo.write(target_position);
    this->interface->send_state("door.state", door_state_to_str(target_state));
    delay(time_to_reach);
    this->interface->send_state("door.position", target_position);
}

bool BasketController::reset_counter(){
    switch (this->door_state) {
        case DoorState::CLOSED:
            return false;
            break;
        case DoorState::OPEN_SMALL:
            this->fill_count.fill_small = 0;
            this->interface->send_state("fill_count.small", this->fill_count.fill_small);
            break;
        case DoorState::OPEN_LARGE:
            this->fill_count.fill_large = 0;
            this->interface->send_state("fill_count.large", this->fill_count.fill_large);
            break;
    }
    return true;
}

int BasketController::get_desired_sorting_pos(SortingState new_sorting_state){
    int desired_pos;

    switch (this->sorting_state){
        case SortingState::IDLE:
            desired_pos = SortingServoValues::idle_pos;
            break;
        case SortingState::SMALL:
            desired_pos = SortingServoValues::small_pos;
            break;
        case SortingState::LARGE:
            desired_pos = SortingServoValues::large_pos;
            break;
    };
    return desired_pos;
}

void BasketController::set_sorting(SortingState target_state){
    int target_position = this->get_desired_sorting_pos(target_state);
    int angular_distance = abs(this->door_servo.read() - target_position);  // angular distance in degrees
    int time_to_reach = (angular_distance * 1000) / SortingServoValues::speed; // [deg]/[deg/s]*[1000ms/s]=[ms]
    this->sorting_state = target_state;
    this->interface->send_state("sorting.state", sorting_state_to_str(target_state));
    this->sorting_servo.write(target_position);
    this->interface->send_state("sorting.position", target_position);
    delay(time_to_reach);
}

bool BasketController::increment_counter(){
    switch (this->sorting_state) {
        case SortingState::IDLE:
            return false;
            break;
        case SortingState::SMALL:
            this->fill_count.fill_small += 1;
            this->interface->send_state("fill_count.small", this->fill_count.fill_small);
            break;
        case SortingState::LARGE:
            this->fill_count.fill_large += 1;
            this->interface->send_state("fill_count.large", this->fill_count.fill_large);
            break;
    }
    return true;
}
