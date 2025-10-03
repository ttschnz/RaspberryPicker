#include <Arduino.h>
#include <Basket.h>

BasketPinout basket_pinout {
  .sorting_pin = 9,
  .door_pin = 10,
};

BasketController* basket_controller;

void setup() {
  basket_controller = new BasketController(&basket_pinout);
}

void loop() {

}
