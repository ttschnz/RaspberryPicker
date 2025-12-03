/**
 * ColorSensor.cpp
 * 
 * RGB color sensor implementation using LDR (Light Dependent Resistor) and RGB LEDs.
 * Measures color by illuminating object with R, G, B LEDs separately and reading reflected light.
 * Includes logistic regression model for raspberry ripeness detection based on color and size.
 */

#include <Arduino.h>
#include "ColorSensor.h"

/**
 * Constructor - initializes color sensor with pin configuration.
 * @param pinout Pin configuration for RGB LEDs and LDR
 */
ColorSensor::ColorSensor(ColorSensor::Pinout pinout)
{
    this->pinout = pinout;

    // Configure RGB LED pins as outputs
    pinMode(pinout.led_r, OUTPUT);
    pinMode(pinout.led_g, OUTPUT);
    pinMode(pinout.led_b, OUTPUT);

    // Configure LDR pin as input
    pinMode(pinout.ldr, INPUT);
}

/**
 * Measures raw RGB color values and ambient light.
 * 
 * Process:
 * 1. Turns on each LED color (R, G, B) sequentially
 * 2. Takes multiple LDR readings and averages them
 * 3. Also measures ambient light (no LED)
 * 
 * @return RAW_RGB structure with red, green, blue, and noise (ambient) values
 */
RAW_RGB ColorSensor::measure_rgb_raw()
{
    int led_pins[] = {
        this->pinout.led_r,
        this->pinout.led_g,
        this->pinout.led_b,
        0 // Special case: measure ambient light with no LED on
    };
    float raw_measurement[] = {0, 0, 0, 0};

    // Measure each color channel plus ambient light
    for (int color_index = 0; color_index < 4; color_index++)
    {
        long long int measurement = 0;

        // Turn on LED if pin is specified (skip for ambient measurement)
        if (led_pins[color_index] > 0)
            digitalWrite(led_pins[color_index], HIGH);
        delay(ColorSensor::delay_color);

        // Take multiple measurements and average for accuracy
        for (int i = 0; i < ColorSensor::measure_count; i++)
        {
            measurement += analogRead(this->pinout.ldr);
            delay(ColorSensor::delay_probe);
        }

        // Turn off LED
        if (led_pins[color_index] > 0)
            digitalWrite(led_pins[color_index], LOW);

        raw_measurement[color_index] = measurement / (float)ColorSensor::measure_count;
    }

    // Package measurements into RAW_RGB structure
    RAW_RGB out_rgb{
        .r = raw_measurement[0],
        .g = raw_measurement[1],
        .b = raw_measurement[2],
        .noise = raw_measurement[3],  // Ambient light reading
    };

    return out_rgb;
}

/**
 * Sigmoid activation function for logistic regression.
 * Maps input to range [0, 1].
 * @param x Input value
 * @return Sigmoid of x, scaled to [0, 1]
 */
float sigmoid(float x)
{
    return 0.5f * (x / (1.0f + fabsf(x)) + 1.0f);
}

// Logistic regression model parameters for ripeness classification
// These were trained on actual raspberry color data
const float logistic_regression_w[] = {-4.50230706, 11.26159486, 3.55473385, -6.83999956, -0.52119049};  // Feature weights
const float logistic_regression_b = -0.2995281290741155;  // Bias term
const float logistic_regression_mean[] = {648.86074332, 440.45632985, 412.50301974, 353.28095238, 25.94192799};  // Feature means for normalization
const float logistic_regression_std[] = {117.48639278, 171.45333209, 172.45127903, 181.21577300, 4.11776362};  // Feature std devs for normalization

/**
 * Calculates the probability that a raspberry is ripe using logistic regression.
 * 
 * Uses a trained logistic regression model that takes into account:
 * - Red, Green, Blue color values
 * - Ambient light (noise)
 * - Width of the raspberry
 * 
 * @param rgb_raw Raw RGB color measurements including ambient light
 * @param width Width of the raspberry in mm
 * @return Probability that the raspberry is ripe (0.0 to 1.0)
 */
float ColorSensor::get_ripenesses_p(RAW_RGB rgb_raw, int width)
{
    // Normalize features using z-score normalization
    float X0 = (rgb_raw.r - (logistic_regression_mean[0])) / (logistic_regression_std[0]);
    float X1 = (rgb_raw.g - (logistic_regression_mean[1])) / (logistic_regression_std[1]);
    float X2 = (rgb_raw.b - (logistic_regression_mean[2])) / (logistic_regression_std[2]);
    float X3 = (rgb_raw.noise - (logistic_regression_mean[3])) / (logistic_regression_std[3]);
    float X4 = (width - (logistic_regression_mean[4])) / (logistic_regression_std[4]);

    // Calculate weighted sum (linear combination)
    float z =
        logistic_regression_w[0] * X0 +
        logistic_regression_w[1] * X1 +
        logistic_regression_w[2] * X2 +
        logistic_regression_w[3] * X3 +
        logistic_regression_w[4] * X4 +
        logistic_regression_b;

    // Apply sigmoid to get probability
    // Label map: 0=ripe, 1=unripe (model predicts unripe)
    float p_hat_unripe = sigmoid(z);

    // Convert to probability of being ripe
    float p_hat_ripe = 1 - p_hat_unripe;

    return p_hat_ripe;
}