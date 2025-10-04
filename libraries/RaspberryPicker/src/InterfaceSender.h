#ifndef RASPBERRY_PICKER_INTERFACE_SENDER_H
#define RASPBERRY_PICKER_INTERFACE_SENDER_H

#include <SoftwareSerial.h>
#include <Arduino.h>

typedef struct {
    int rx_pin;
    int tx_pin;
    int baud_rate;
} InterfaceConfiguration;

class InterfaceSender {
    public:
        InterfaceSender(InterfaceConfiguration* config);
        template <typename T>
            void send_state(const char* key, T value) {
                String str_value = String(value); // Convert value to String internally
                String data_buffer;
                data_buffer.concat(key);
                data_buffer.concat("=");
                data_buffer.concat(str_value);

                this->Serial->println(data_buffer);
            };
    private:
        SoftwareSerial* Serial;
};

#endif
