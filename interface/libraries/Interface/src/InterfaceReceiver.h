#ifndef RASPBERRY_INTERFACE_RECEIVER_H
#define RASPBERRY_INTERFACE_RECEIVER_H
#include <ArduinoJson.h>

class InterfaceReceiver {
    public:
        InterfaceReceiver();
        void listen();
        String get_state_json();
        void request_state_change(String key, String value);
    private:
        JsonDocument state;
};

#endif
