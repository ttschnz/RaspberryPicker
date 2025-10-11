#ifndef RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_H
#define RASPBERRY_PICKER_GRIPPER_COLOR_SENSOR_H
struct RGB{
    float r;
    float g;
    float b;
};

class ColorSensor
{
    public:
        struct Pinout{
            int led_r;
            int led_g;
            int led_b;
            int ldr;
        };

        static const int measure_count;  // how many measurements should be taken to average?
        static const int delay_probe; // delay before probing another measurement
        static const int delay_color; // delay before switching color
        static const int delay_calibrate; // how long do we take to switch from white to black?
        ColorSensor(Pinout pinout);
        void calibrate();
        RGB measure_rgb();

    private:
        void measure_rgb_raw(float* out_r, float* out_g, float*out_b);
        RGB white;
        RGB black;
        Pinout pinout;
};

#endif
