#include "InterfaceServer.h"
#include "InterfaceSlave.h"

InterfaceServer* interface_server;
InterfaceSlave* interface_slave;

void setup() {
    delay(1000);
    interface_slave = new InterfaceSlave();
    interface_server = new InterfaceServer(interface_slave);
}

void loop() {
    interface_slave->listen();
}
