#include "InterfaceServer.h"
#include "InterfaceReceiver.h"

InterfaceServer* interface_server;
InterfaceReceiver* interface_receiver;

void setup() {
    delay(1000);
    interface_receiver = new InterfaceReceiver();
    interface_server = new InterfaceServer(interface_receiver);
}

void loop() {
    interface_receiver->listen();
}
