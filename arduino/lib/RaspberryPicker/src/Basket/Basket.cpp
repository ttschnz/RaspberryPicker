#include <Arduino.h>
#include <Servo.h>

#include "../InterfaceMaster.h"

#include "Basket.h"
#include "Door.h"
#include "Sorting.h"

const int BasketSorter::small_pos = 20;
const int BasketSorter::idle_pos = 86;
const int BasketSorter::large_pos = 152;

const int BasketDoor::closed_pos = 170;
const int BasketDoor::open_pos = 10;
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
        case BasketDoor::DoorState::OPEN:
            desired_pos = BasketDoor::open_pos;
            break;
    };
    Serial.println((String)"desired door pos: " + desired_pos);
    return desired_pos;
}

void BasketController::set_door(BasketDoor::DoorState target_state){
    int target_position = this->get_desired_door_pos(target_state);
    this->door_state = target_state;
    this->door_servo.write(target_position);
    this->interface->send_state("basket.door.state", BasketDoor::serialize_door_state(target_state));
    this->interface->send_state("basket.door.position", target_position);
}

bool BasketController::reset_counter(bool force){
    bool reset = (this->door_state == BasketDoor::DoorState::OPEN) || force;

    if (reset){
            this->fill_count.fill_small = 0;
            this->interface->send_state("basket.fill_count.small", this->fill_count.fill_small);
            this->fill_count.fill_large = 0;
            this->interface->send_state("basket.fill_count.large", this->fill_count.fill_large);
    }

    return reset;
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
    this->sorting_state = target_state;
    this->interface->send_state("basket.sorting.state", BasketSorter::serialize_sorting_state(target_state));
    this->sorting_servo.write(target_position);
    this->interface->send_state("basket.sorting.position", target_position);
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
