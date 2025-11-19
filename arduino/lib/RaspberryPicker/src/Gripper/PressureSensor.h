#ifndef RASPBERRY_PICKER_GRIPPER_PRESSURE_SENSOR_H
#define RASPBERRY_PICKER_GRIPPER_PRESSURE_SENSOR_H

class PressureSensor{
    public:
        PressureSensor(int pinout[1]);

        static const float berry_size_threshold;               // Threshold value to distinguish large berries from small ones (in mm)
        static const int pressure_sensor_thresholds[2];        // Threshold values when we consider the contact as touching (in 5V/1023) [0,1023]

        enum class PressureState{
            TOUCHING,
            NOT_TOUCHING
        };


        /**
         * turns the state (enum) to a string
         */
        static const char* serialize_pressure_state(PressureState pressure_state);

        /**
         * Parses the state (string) to an instance of its enum. returns false if there is no match.
         */
        static bool deserialize_pressure_state(String pressure_state_str, PressureState* out_pressure_state);

        /**
         * returns true if the pressure sensor's reading is high enough
         * (i.e. we think the plates touch a berry)
         * returns false if this is not the case.
         */
        bool is_touching(bool log);
    private:
        int pinout[1];
};

#endif
