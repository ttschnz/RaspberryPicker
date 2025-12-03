/**
 * ColorSensor.h
 * 
 * RGB color sensor interface using LDR (Light Dependent Resistor) and RGB LEDs.
 * Measures color by illuminating object with R, G, B LEDs separately and reading reflected light.
 * Includes logistic regression model for raspberry ripeness detection based on color and size.
 */

#ifndef RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_H
#define RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_H

/**
 * RAW_RGB structure - raw color measurements including ambient light.
 * r: Red channel reading
 * g: Green channel reading
 * b: Blue channel reading
 * noise: Ambient light reading (no LED active)
 */
struct RAW_RGB
{
    float r;
    float g;
    float b;
    float noise;
};

/**
 * RGB structure - normalized color values.
 * r: Red component
 * g: Green component
 * b: Blue component
 */
struct RGB
{
    float r;
    float g;
    float b;
};

/**
 * ColorSensor class - manages RGB color sensing and ripeness detection.
 * Uses RGB LEDs and LDR to measure reflected light for color analysis.
 */
class ColorSensor
{
public:
    /**
     * Pinout structure - pin assignments for color sensor components.
     * led_r: Digital pin for red LED
     * led_g: Digital pin for green LED
     * led_b: Digital pin for blue LED
     * ldr: Analog pin for Light Dependent Resistor
     */
    struct Pinout
    {
        int led_r;
        int led_g;
        int led_b;
        int ldr;
    };

    static const int measure_count; // Number of measurements to average per color
    static const int delay_probe;   // Delay between individual measurements [ms]
    static const int delay_color;   // Delay after LED activation before measurement [ms]

    /**
     * Constructor - initializes the color sensor with pin configuration.
     * Call this during setup.
     */
    ColorSensor(Pinout pinout);

    /**
     * Measures red, green, and blue channels separately.
     * Returns raw reading values regardless of calibration status.
     * Also measures ambient light (noise) with all LEDs off.
     * @return RAW_RGB structure with color and ambient measurements
     */
    RAW_RGB measure_rgb_raw();
    
    /**
     * Calculates the probability that a raspberry is ripe using logistic regression.
     * Takes into account color values (RGB + ambient light) and raspberry width.
     * @param rgb_raw Raw RGB color measurements
     * @param width Width of the raspberry [mm]
     * @return Probability that raspberry is ripe (0.0 to 1.0)
     */
    float get_ripenesses_p(RAW_RGB rgb_raw, int width);

private:
    Pinout pinout;  // Pin configuration for LEDs and LDR
};

#endif
