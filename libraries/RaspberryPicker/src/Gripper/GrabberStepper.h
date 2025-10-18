#ifndef RASPBERRY_PICKER_GRIPPER_PLATE_H
#define RASPBERRY_PICKER_GRIPPER_PLATE_H

#ifndef PI
#define PI 3.141592653589793
#endif

enum class GrabberState {
    OPEN,
    CLOSED,
};

static const char* grabber_state_strings[2] = {
    "OPEN",
    "CLOSED"
};

const char* grabber_state_to_str(GrabberState grabber_state);
bool str_to_grabber_state(String grabber_state_str, GrabberState* out_grabber_state);


class GrabberStepperMotorValues
{
    public:
        static const int steps_per_revolution; // amount of steps per revolution
        static const int speed;                // speed (RPM) to use while driving
        static const float transmission_ratio; // how many mm does the plate move per rotation? [mm/rot] module * Z * pi
        static const int plate_distance;       // how many mm do we want the plates to be apart from each other in the open state?
        static const int plate_distance_min;   // what is the minimal distance we should keep between plates? [mm]
        static const int measuring_interval;   // how many mm do we move until we check if we have pressure?
};

#endif
