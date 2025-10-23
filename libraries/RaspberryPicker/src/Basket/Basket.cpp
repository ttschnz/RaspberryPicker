#include <Arduino.h>
#include <Servo.h>

#include "../InterfaceMaster.h"

#include "Basket.h"
#include "Door.h"
#include "Sorting.h"

const int BasketSorter::small_pos = 0;
const int BasketSorter::idle_pos = 90;
const int BasketSorter::large_pos = 180;
const int BasketSorter::speed = 500;

const int BasketDoor::open_small_pos = 0;
const int BasketDoor::closed_pos = 90;
const int BasketDoor::open_large_pos = 180;
const int BasketDoor::speed = 300;
const int BasketDoor::max_fill = 23;

BasketController::BasketController(BasketPinout *pinout,InterfaceMaster *interface){
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
    this->set_sorting(BasketSorter::SortingState::IDLE);
    this->set_door(BasketDoor::DoorState::CLOSED);
}

int BasketController::get_desired_door_pos(BasketDoor::DoorState new_door_state){
    int desired_pos;

    switch (new_door_state){
        case BasketDoor::DoorState::CLOSED:
            desired_pos = BasketDoor::closed_pos;
            break;
        case BasketDoor::DoorState::OPEN_SMALL:
            desired_pos = BasketDoor::open_small_pos;
            break;
        case BasketDoor::DoorState::OPEN_LARGE:
            desired_pos = BasketDoor::open_large_pos;
            break;
    };
    Serial.println((String)"desired door pos: " + desired_pos);
    return desired_pos;
}

void BasketController::set_door(BasketDoor::DoorState target_state){
    int target_position = this->get_desired_door_pos(target_state);
    int angular_distance = abs(this->door_servo.read() - target_position);  // angular distance in degrees
    int time_to_reach = (angular_distance * 1000) / BasketDoor::speed; // [deg]/[deg/s]*[1000ms/s]=[ms]
    this->door_state = target_state;
    this->door_servo.write(target_position);
    this->interface->send_state("basket.door.state", BasketDoor::serialize_door_state(target_state));
    delay(time_to_reach);
    this->interface->send_state("basket.door.position", target_position);
}

bool BasketController::reset_counter(bool force){
    bool reset_large = (this->door_state == BasketDoor::DoorState::OPEN_LARGE) || force;
    bool reset_small = (this->door_state == BasketDoor::DoorState::OPEN_SMALL) || force;

    if (reset_small){
            this->fill_count.fill_small = 0;
            this->interface->send_state("basket.fill_count.small", this->fill_count.fill_small);
    }

    if(reset_large){
            this->fill_count.fill_large = 0;
            this->interface->send_state("basket.fill_count.large", this->fill_count.fill_large);
    }

    return reset_large || reset_small;
}

int BasketController::get_desired_sorting_pos(BasketSorter::SortingState new_sorting_state){
    int desired_pos;

    switch (new_sorting_state){
        case BasketSorter::SortingState::IDLE:
            desired_pos = BasketSorter::idle_pos;
            break;
        case BasketSorter::SortingState::SMALL:
            desired_pos = BasketSorter::small_pos;
            break;
        case BasketSorter::SortingState::LARGE:
            desired_pos = BasketSorter::large_pos;
            break;
    };
    return desired_pos;
}

void BasketController::set_sorting(BasketSorter::SortingState target_state){
    int target_position = this->get_desired_sorting_pos(target_state);
    int angular_distance = abs(this->door_servo.read() - target_position);  // angular distance in degrees
    int time_to_reach = (angular_distance * 1000) / BasketSorter::speed; // [deg]/[deg/s]*[1000ms/s]=[ms]
    this->sorting_state = target_state;
    this->interface->send_state("basket.sorting.state", BasketSorter::serialize_sorting_state(target_state));
    this->sorting_servo.write(target_position);
    this->interface->send_state("basket.sorting.position", target_position);
    delay(time_to_reach);
}

bool BasketController::increment_counter(){
    switch (this->sorting_state) {
        case BasketSorter::SortingState::IDLE:
            return false;
            break;
        case BasketSorter::SortingState::SMALL:
            this->fill_count.fill_small += 1;
            this->interface->send_state("basket.fill_count.small", this->fill_count.fill_small);
            break;
        case BasketSorter::SortingState::LARGE:
            this->fill_count.fill_large += 1;
            this->interface->send_state("basket.fill_count.large", this->fill_count.fill_large);
            break;
    }
    return true;
}
