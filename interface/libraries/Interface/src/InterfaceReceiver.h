#ifndef RASPBERRY_INTERFACE_RECEIVER_H
#define RASPBERRY_INTERFACE_RECEIVER_H
#include <ArduinoJson.h>

class InterfaceReceiver {
    public:
        InterfaceReceiver();
        void listen();
        String get_state_json();
    private:
        JsonDocument state;
};

#endif
