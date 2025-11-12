#ifndef RASPBERRY_PICKER_GRIPPER_PLATE_H
#define RASPBERRY_PICKER_GRIPPER_PLATE_H

#ifndef PI
#define PI 3.141592653589793
#endif

static const char* gripper_state_strings[2] = {
    "OPEN",
    "CLOSED"
};

static const char* raspberry_size_strings[] = {
    "LARGE",
    "SMALL"
};

class GripperStepper
{

    public:
        enum RaspberrySize{
            LARGE,
            SMALL
        };

        static const char* serialize_raspberry_size(GripperStepper::RaspberrySize raspberry_size);

        static bool deserialize_raspberry_size(String raspberry_size_str, GripperStepper::RaspberrySize* out_raspberry_size);


        enum class GripperState {
            OPEN,
            CLOSED_SMALL,
            CLOSED_LARGE,
        };

        static const char* serialize_gripper_state(GripperStepper::GripperState gripper_state);

        static bool deserialize_gripper_state(String gripper_state_str, GripperStepper::GripperState* out_gripper_state);

        static int get_desired_step_position(GripperState state);
        static int mm_to_steps(int mm);
        static int steps_to_mm(int steps);

        static const int steps_per_revolution; // amount of steps per revolution
        static const int speed;                // speed (RPM) to use while run
        static const int max_speed;            // max speed (RPM) to use while moveTo
        static const int acceleration;         // acceleration to use while moveTo: keep around 300 to maintain control
        static const float transmission_ratio; // how many mm does the plate move per rotation? [mm/rot] module * Z * pi
        static const int plate_distance_open;  // how many mm do we want the plates to be apart from each other in the open state?
        static const int plate_distance_large; // how many mm are the plates apart from each other for a large berry?
        static const int plate_distance_small; // how many mm are the plates apart from each other for a small berry?
        static const int plate_distance_limit; // how many mm are the plates apart from each other when the limit switch is pressed?
};

#endif
