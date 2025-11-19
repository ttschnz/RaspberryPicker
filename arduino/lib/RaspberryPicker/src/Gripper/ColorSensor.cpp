#include <Arduino.h>
#include "ColorSensor.h"

ColorSensor::ColorSensor(ColorSensor::Pinout pinout){
    this->pinout = pinout;

    pinMode(pinout.led_r, OUTPUT);
    pinMode(pinout.led_g, OUTPUT);
    pinMode(pinout.led_b, OUTPUT);

    pinMode(pinout.ldr, INPUT);

    this->black = RGB{0,0,0};
    this->white = RGB{1,1,1};
}

void ColorSensor::measure_rgb_raw(float* out_r, float* out_g, float*out_b){
    int led_pins[3] = {
        this->pinout.led_r,
        this->pinout.led_g,
        this->pinout.led_b,
    };
    float raw_measurement[3] = {0,0,0};


    for (int color_index = 0; color_index < 3; color_index++){
        long long int measurement = 0;
        digitalWrite(led_pins[color_index], HIGH);
        for (int i = 0; i < ColorSensor::measure_count; i++){
            measurement+=analogRead(this->pinout.ldr);
            delay(ColorSensor::delay_probe);
        }
        digitalWrite(led_pins[color_index], LOW);
        raw_measurement[color_index] = measurement / (float)ColorSensor::measure_count;
        delay(ColorSensor::delay_color);
    }

    *out_r = raw_measurement[0];
    *out_g = raw_measurement[1];
    *out_b = raw_measurement[2];

    return;
}

RGB ColorSensor::measure_rgb(){
    RGB raw_rgb;
    this->measure_rgb_raw(&raw_rgb.r, &raw_rgb.g, &raw_rgb.b);

    RGB normalised = RGB{
        (raw_rgb.r - this->black.r) / (this->white.r - this->black.r),
        (raw_rgb.g - this->black.g) / (this->white.g - this->black.g),
        (raw_rgb.b - this->black.b) / (this->white.b - this->black.b),
    };

    // avoid negative values
    normalised.r = normalised.r < 0 ? 0 : normalised.r;
    normalised.g = normalised.g < 0 ? 0 : normalised.g;
    normalised.b = normalised.b < 0 ? 0 : normalised.b;

    return normalised;
}



void ColorSensor::calibrate(){
    Serial.println("calibrating white");
    delay(ColorSensor::delay_calibrate);
    RGB raw_white;
    this->measure_rgb_raw(&raw_white.r, &raw_white.g, &raw_white.b);

    Serial.println("calibrating black");
    delay(ColorSensor::delay_calibrate);
    RGB raw_black;
    this->measure_rgb_raw(&raw_black.r, &raw_black.g, &raw_black.b);

    this->white = raw_white;
    this->black = raw_black;
    Serial.println((String)"gripper.color.white_calibration=" + raw_white.r + "/" + raw_white.g + "/" + raw_white.b);
    Serial.println((String)"gripper.color.black_calibration=" + raw_black.r + "/" + raw_black.g + "/" + raw_black.b);
    
    Serial.println("calibrated");
}
