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
        static const int delay_calibrate; // how long do we take to switch from white to black?

        static const double logistic_regression_w[4]; // weight vector for logistic regression
        static const double logistic_regression_b; // bias for logistic regression
        static const double logistic_regression_mean[4];
        static const double logistic_regression_std[4];
        /**
         * Initialises the color sensor. Call this during setup
         */
        ColorSensor(Pinout pinout);

        /**
         * calibrates the color sensor. After this, values will be normalised
         * linearliy with respect to the reference values measured during this
         * calibration.
         *
         * At first we calibrate the white measurement, then black.
         */
        void calibrate();

        /**
         * Measures the red green and blue channel separately. If the sensor is
         * calibrated, the readings are returned normalised (normally bewteen 0
         * and 1), otherwise the raw reading is returned.
         */
        RGB measure_rgb();
        
        /**
        * Measures the red green and blue channel separately. This returns the
        * raw reading values no matter wether the sensor is calibrated or not.
        */
        RAW_RGB measure_rgb_raw();

        double get_ripenesses_p(RAW_RGB rgb_raw);
    private:

        // reference white reading
        RAW_RGB white;
        // reference black reading
        RAW_RGB black;
        Pinout pinout;
};

#endif
