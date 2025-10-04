#include "InterfaceReceiver.h"
#include <Arduino.h>

InterfaceReceiver::InterfaceReceiver(InterfacePinout* pinout){
    pinMode(pinout->rx, INPUT);
    pinMode(pinout->tx, OUTPUT);
};
