#ifndef RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_H
#define RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_H

struct RAW_RGB{
    float r;
    float g;
    float b;
    float noise;
};

struct RGB{
    float r;
    float g;
    float b;
};

class ColorSensor
{
    public:
        /**
        * Pinout for the led (one digital out for each color) and ldr sensor (one analog in)
        */
        struct Pinout{
            int led_r;
            int led_g;
            int led_b;
            int ldr;
        };

        static const int measure_count;   // how many measurements should be taken to average?
        static const int delay_probe;     // delay before probing another measurement
        static const int delay_color;     // delay before switching color

        /**
         * Initialises the color sensor. Call this during setup
         */
        ColorSensor(Pinout pinout);

        /**
        * Measures the red green and blue channel separately. This returns the
        * raw reading values no matter wether the sensor is calibrated or not.
        */
        RAW_RGB measure_rgb_raw();

        float get_ripenesses_p(RAW_RGB rgb_raw);
    private:

        Pinout pinout;
};

#endif
