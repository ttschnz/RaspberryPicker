#include <Arduino.h>
#include "ColorSensor.h"

ColorSensor::ColorSensor(ColorSensor::Pinout pinout){
    this->pinout = pinout;

    pinMode(pinout.led_r, OUTPUT);
    pinMode(pinout.led_g, OUTPUT);
    pinMode(pinout.led_b, OUTPUT);

    pinMode(pinout.ldr, INPUT);

    this->black = RAW_RGB{0,0,0,0};
    this->white = RAW_RGB{1,1,1,1};
}

RAW_RGB ColorSensor::measure_rgb_raw(){
    int led_pins[] = {
        this->pinout.led_r,
        this->pinout.led_g,
        this->pinout.led_b,
        0 // special case: we also want to measure ambient light. we'll check if the pin is zero
    };
    float raw_measurement[] = {0,0,0,0};


    for (int color_index = 0; color_index < 4; color_index++){
        long long int measurement = 0;
        
        if (led_pins[color_index] > 0)
            digitalWrite(led_pins[color_index], HIGH);
        delay(ColorSensor::delay_color);

        for (int i = 0; i < ColorSensor::measure_count; i++){
            measurement+=analogRead(this->pinout.ldr);
            delay(ColorSensor::delay_probe);
        }

        if (led_pins[color_index] > 0)
            digitalWrite(led_pins[color_index], LOW);

        raw_measurement[color_index] = measurement / (float)ColorSensor::measure_count;
    }

    RAW_RGB out_rgb{
        .r=raw_measurement[0],
        .g=raw_measurement[1],
        .b=raw_measurement[2],
        .noise=raw_measurement[3],
    };
    
    return out_rgb;
}

RGB ColorSensor::measure_rgb(){
    RAW_RGB raw_rgb = this->measure_rgb_raw();

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
    RAW_RGB raw_white = this->measure_rgb_raw();

    Serial.println("calibrating black");
    delay(ColorSensor::delay_calibrate);
    RAW_RGB raw_black = this->measure_rgb_raw();

    this->white = raw_white;
    this->black = raw_black;
    Serial.println((String)"gripper.color.white_calibration=" + raw_white.r + "/" + raw_white.g + "/" + raw_white.b);
    Serial.println((String)"gripper.color.black_calibration=" + raw_black.r + "/" + raw_black.g + "/" + raw_black.b);
    
    Serial.println("calibrated");
}


// double sigmoid(double z){
//     double s = 1/(1+exp(-z));
//     return s;
// }
double sigmoid(double z){
    if (z > 50) return 1.0;
    if (z < -50) return 0.0;
    if (z >= 0){
        double e = exp(-z);
        return 1.0 / (1.0 + e);
    } else {
        double e = exp(z);
        return e / (1.0 + e);
    }
}

void normalize(double X[4]){
    const double *mean = ColorSensor::logistic_regression_mean;
    const double *std = ColorSensor::logistic_regression_std;
    for(int i = 0; i<4; i++){
        X[i] = (X[i] - mean[i]) / std[i];
    }
}

double ColorSensor::get_ripenesses_p(RAW_RGB rgb_raw){
    const double *w = ColorSensor::logistic_regression_w;
    const double b = ColorSensor::logistic_regression_b;

    double X[] = {rgb_raw.r, rgb_raw.g, rgb_raw.b, rgb_raw.noise};
    normalize(X);

    double z = w[0]*X[0] + w[1]*X[1] + w[2]*X[2] + w[3]*X[3] + b;

    // label map is 0:ripe, 1:unripe
    double p_hat_unripe = sigmoid(z);

    double p_hat_ripe = 1-p_hat_unripe;

    return p_hat_ripe;    
}