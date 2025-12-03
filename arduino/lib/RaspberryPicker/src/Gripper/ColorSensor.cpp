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

const float logistic_regression_w[] = {-4.50230706, 11.26159486, 3.55473385, -6.83999956, -0.52119049};
const float logistic_regression_b = -0.2995281290741155;
const float logistic_regression_mean[] = {648.86074332, 440.45632985, 412.50301974, 353.28095238, 25.94192799};
const float logistic_regression_std[] = {117.48639278, 171.45333209, 172.45127903, 181.21577300, 4.11776362};

float ColorSensor::get_ripenesses_p(RAW_RGB rgb_raw, int width)
{

    float X0 = (rgb_raw.r - (logistic_regression_mean[0])) / (logistic_regression_std[0]);
    float X1 = (rgb_raw.g - (logistic_regression_mean[1])) / (logistic_regression_std[1]);
    float X2 = (rgb_raw.b - (logistic_regression_mean[2])) / (logistic_regression_std[2]);
    float X3 = (rgb_raw.noise - (logistic_regression_mean[3])) / (logistic_regression_std[3]);
    float X4 = (width - (logistic_regression_mean[4])) / (logistic_regression_std[4]);

    float z =
        logistic_regression_w[0] * X0 +
        logistic_regression_w[1] * X1 +
        logistic_regression_w[2] * X2 +
        logistic_regression_w[3] * X3 +
        logistic_regression_w[4] * X4 +
        logistic_regression_b;

    // label map is 0:ripe, 1:unripe
    float p_hat_unripe = sigmoid(z);

    float p_hat_ripe = 1 - p_hat_unripe;

    return p_hat_ripe;
}