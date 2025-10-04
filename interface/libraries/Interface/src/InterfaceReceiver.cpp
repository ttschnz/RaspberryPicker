#include <Arduino.h>
#include <ArduinoJson.h>

#include "InterfaceReceiver.h"

InterfaceReceiver::InterfaceReceiver(){
    Serial.begin(9600);
};

void InterfaceReceiver::listen(){
    if (Serial.available() > 0) {
        String line = Serial.readStringUntil('\n');  // Read a line ending with newline
        line.trim(); // Remove any trailing whitespace or \r

        int delimiterPos = line.indexOf('=');
        if (delimiterPos > 0) {
          String key = line.substring(0, delimiterPos);
          String value = line.substring(delimiterPos + 1);

          this->state[key] = value;
        }
      }
}

void InterfaceReceiver::request_state_change(String key, String value){
    String data_buffer;
    data_buffer.concat(key);
    data_buffer.concat("=");
    data_buffer.concat(value);

    Serial.println(data_buffer);
}

String InterfaceReceiver::get_state_json(){
    String state;
    serializeJsonPretty(this->state, state);
    return state;
}
