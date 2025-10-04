#include "InterfaceSender.h"
#include <Arduino.h>

InterfaceSender::InterfaceSender(InterfacePinout* pinout){
    pinMode(pinout->rx, INPUT);
    pinMode(pinout->tx, OUTPUT);
};
