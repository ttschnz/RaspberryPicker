#include "InterfaceSender.h"

#include <SoftwareSerial.h>
#include <Arduino.h>

InterfaceSender::InterfaceSender(InterfaceConfiguration* config){
    pinMode(config->rx_pin, INPUT);
    pinMode(config->tx_pin, OUTPUT);

    this->Serial = new SoftwareSerial(config->rx_pin, config->tx_pin);
    this->Serial->begin(config->baud_rate);
};
