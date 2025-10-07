#ifndef RASPBERRY_INTERFACE_SLAVE_H
#define RASPBERRY_INTERFACE_SLAVE_H
#include <ArduinoJson.h>

class InterfaceSlave {
    public:
        InterfaceSlave();
        void listen();
        String get_state_json();
        void request_state_change(String key, String value);
    private:
        JsonDocument state;
};

#endif
