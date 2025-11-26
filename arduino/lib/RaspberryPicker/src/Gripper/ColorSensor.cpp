#include <Arduino.h>
#include "ColorSensor.h"

ColorSensor::ColorSensor(ColorSensor::Pinout pinout)
{
    this->pinout = pinout;

    pinMode(pinout.led_r, OUTPUT);
    pinMode(pinout.led_g, OUTPUT);
    pinMode(pinout.led_b, OUTPUT);

    pinMode(pinout.ldr, INPUT);
}

RAW_RGB ColorSensor::measure_rgb_raw()
{
    int led_pins[] = {
        this->pinout.led_r,
        this->pinout.led_g,
        this->pinout.led_b,
        0 // special case: we also want to measure ambient light. we'll check if the pin is zero
    };
    float raw_measurement[] = {0, 0, 0, 0};

    for (int color_index = 0; color_index < 4; color_index++)
    {
        long long int measurement = 0;

        if (led_pins[color_index] > 0)
            digitalWrite(led_pins[color_index], HIGH);
        delay(ColorSensor::delay_color);

        for (int i = 0; i < ColorSensor::measure_count; i++)
        {
            measurement += analogRead(this->pinout.ldr);
            delay(ColorSensor::delay_probe);
        }

        if (led_pins[color_index] > 0)
            digitalWrite(led_pins[color_index], LOW);

        raw_measurement[color_index] = measurement / (float)ColorSensor::measure_count;
    }

    RAW_RGB out_rgb{
        .r = raw_measurement[0],
        .g = raw_measurement[1],
        .b = raw_measurement[2],
        .noise = raw_measurement[3],
    };

    return out_rgb;
}

float sigmoid(float x)
{
    return 0.5f * (x / (1.0f + fabsf(x)) + 1.0f);
}

const float logistic_regression_w[4] = {-2.82726153, -8.91658046, 52.32324727, -38.16793768};
const float logistic_regression_b = -1.2026309385016440;
const float logistic_regression_mean[4] = {689.88443114, 587.60494012, 578.94408683, 554.74618263};
const float logistic_regression_std[4] = {96.15270717, 186.68595658, 192.60221362, 203.43980064};

float ColorSensor::get_ripenesses_p(RAW_RGB rgb_raw)
{

    float X0 = (rgb_raw.r - (logistic_regression_mean[0])) / (logistic_regression_std[0]);
    float X1 = (rgb_raw.g - (logistic_regression_mean[1])) / (logistic_regression_std[1]);
    float X2 = (rgb_raw.b - (logistic_regression_mean[2])) / (logistic_regression_std[2]);
    float X3 = (rgb_raw.noise - (logistic_regression_mean[3])) / (logistic_regression_std[3]);

    float z =
        logistic_regression_w[0] * X0 +
        logistic_regression_w[1] * X1 +
        logistic_regression_w[2] * X2 +
        logistic_regression_w[3] * X3 +
        logistic_regression_b;

    // label map is 0:ripe, 1:unripe
    float p_hat_unripe = sigmoid(z);

    float p_hat_ripe = 1 - p_hat_unripe;

    return p_hat_ripe;
}