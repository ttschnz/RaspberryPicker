#include "InterfaceServer.h"
#include "InterfaceReceiver.h"

InterfaceServer* interface_server;
InterfaceReceiver* interface_receiver;

InterfacePinout interface_pinout{
    rx: 9,
    tx: 10
};

void setup() {
    delay(1000);
    interface_server = new InterfaceServer();
    interface_receiver = new InterfaceReceiver(&interface_pinout);
}

void loop() {
    delay(1000);
}
