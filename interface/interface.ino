#include "Interface.h"

Interface* interface;

InterfacePinout interface_pinout{
    rx: 9,
    tx: 10
};

void setup() {
    delay(1000);
    interface = new Interface(&interface_pinout);
}

void loop() {
    delay(1000);
}
