#ifndef RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_3LDR_H
#define RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_3LDR_H


struct RGB_3LDR{
    float r;
    float g;
    float b;
};

struct RAW_RGB_3LDR{
    float ambient[3];
    float illuminated[3];
};
class ColorSensor3LDR
{
    public:
        /**
        * Pinout for the led (one digital out for each color) and ldr sensor (one analog in)
        */
        struct Pinout{
            int ldr_r;
            int ldr_g;
            int ldr_b;
            int led;
        };

        static const int measure_count;   // how many measurements should be taken to average?
        static const int delay_probe;     // delay before probing another measurement
        
        /**
         * Initialises the color sensor. Call this during setup
         */
        ColorSensor3LDR(Pinout pinout);

        /**
         * Measures the red green and blue channel separately. If the sensor is
         * calibrated, the readings are returned normalised (normally bewteen 0
         * and 1), otherwise the raw reading is returned.
         */
        RGB_3LDR measure_rgb();
        
        /**
        * Measures the red green and blue channel separately. This returns the
        * raw reading values no matter wether the sensor is calibrated or not.
        */
        RAW_RGB_3LDR measure_rgb_raw();
    private:
        Pinout pinout;
};

#endif
