#ifndef RASPBERRY_PICKER_GRIPPER_PRESSURE_SENSOR_H
#define RASPBERRY_PICKER_GRIPPER_PRESSURE_SENSOR_H

static const char* pressure_state_strings[] = {
    "TOUCHING",
    "NOT_TOUCHING",
};

class PressureSensor{
    public:
        PressureSensor(int pinout[2]);

        static const float berry_size_small_standard_deviation;  // Standard deviation of the small berries (in mm)
        static const float berry_size_small_mean;                // Mean of the small berries (in mm)
        static const float berry_size_large_standard_deviation;  // Standard deviation of the large berries (in mm)
        static const float berry_size_large_mean;                // Mean of the large berries (in mm)
        static const int pressure_sensor_thresholds[2];        // Threshold values when we consider the contact as touching (in 5V/1023) [0,1023]

        enum class PressureState{
            TOUCHING,
            NOT_TOUCHING
        };



        static const char* pressure_state_to_str(PressureState pressure_state);
        static bool str_to_pressure_state(String pressure_state_str, PressureState* out_pressure_state);

        bool is_touching();
    private:
        int pinout[2];
};

#endif
