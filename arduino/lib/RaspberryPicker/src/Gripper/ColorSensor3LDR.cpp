#include <Arduino.h>
#include "ColorSensor3LDR.h"

ColorSensor3LDR::ColorSensor3LDR(ColorSensor3LDR::Pinout pinout){
    this->pinout = pinout;

    pinMode(pinout.ldr_r, INPUT);
    pinMode(pinout.ldr_g, INPUT);
    pinMode(pinout.ldr_b, INPUT);

    pinMode(pinout.led, OUTPUT);
}

RAW_RGB_3LDR ColorSensor3LDR::measure_rgb_raw(){
    int ldr_pins[] = {
        this->pinout.ldr_r,
        this->pinout.ldr_g,
        this->pinout.ldr_b,
    };
    RAW_RGB_3LDR measurement{0,0,0,0,0,0};
    // measure colors without LED
    for (int i = 0; i < ColorSensor3LDR::measure_count; i++){
        for (int color_index = 0; color_index < 3; color_index++){
            measurement.ambient[color_index]+=analogRead(ldr_pins[color_index]);
        }
        delay(ColorSensor3LDR::delay_probe);
    }

    // measure_colors with LED
    digitalWrite(this->pinout.led, HIGH);
    delay(ColorSensor3LDR::delay_led);
    for (int i = 0; i < ColorSensor3LDR::measure_count; i++){
        for (int color_index = 0; color_index < 3; color_index++){
            measurement.illuminated[color_index]+=analogRead(ldr_pins[color_index]);
        }
        delay(ColorSensor3LDR::delay_probe);
    }
    digitalWrite(this->pinout.led, LOW);

    
    return measurement;
}

RGB_3LDR ColorSensor3LDR::measure_rgb(){
    RAW_RGB_3LDR raw_rgb = this->measure_rgb_raw();

    RGB_3LDR normalised = RGB_3LDR{
        .r=raw_rgb.illuminated[0] - raw_rgb.ambient[0],
        .g=raw_rgb.illuminated[1] - raw_rgb.ambient[1],
        .b=raw_rgb.illuminated[2] - raw_rgb.ambient[2],
    };

    return normalised;
}



