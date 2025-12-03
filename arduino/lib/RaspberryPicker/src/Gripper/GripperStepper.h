/**
 * GripperStepper.h
 * 
 * Gripper stepper motor state management and unit conversion utilities.
 * Provides serialization/deserialization of gripper and raspberry size states,
 * and conversion functions between millimeters and stepper motor steps.
 */

#ifndef RASPBERRY_PICKER_GRIPPER_PLATE_H
#define RASPBERRY_PICKER_GRIPPER_PLATE_H

#ifndef PI
#define PI 3.141592653589793
#endif

/**
 * GripperStepper class - manages gripper stepper motor state and conversions.
 * Provides utilities for state serialization and distance calculations.
 */
class GripperStepper
{

public:
    /**
     * RaspberrySize enum - detected size of raspberry in gripper.
     * LARGE: Large raspberry detected
     * SMALL: Small raspberry detected
     * UNKNOWN: No raspberry detected or size undetermined
     */
    enum RaspberrySize
    {
        LARGE,
        SMALL,
        UNKNOWN,
    };

    /**
     * Converts RaspberrySize enum to string representation.
     */
    static const char *serialize_raspberry_size(GripperStepper::RaspberrySize raspberry_size);

    /**
     * Converts string to RaspberrySize enum.
     */
    static bool deserialize_raspberry_size(String raspberry_size_str, GripperStepper::RaspberrySize *out_raspberry_size);

    /**
     * GripperState enum - operational states of the gripper.
     * OPEN: Gripper fully open
     * CLOSED_SMALL: Gripper closed to small raspberry position
     * CLOSED_LARGE: Gripper closed to large raspberry position
     * CLOSED_LIMIT: Gripper closed to limit switch position
     */
    enum class GripperState
    {
        OPEN,
        CLOSED_SMALL,
        CLOSED_LARGE,
        CLOSED_LIMIT,
    };

    /**
     * Converts GripperState enum to string representation.
     */
    static const char *serialize_gripper_state(GripperStepper::GripperState gripper_state);

    /**
     * Converts string to GripperState enum.
     */
    static bool deserialize_gripper_state(String gripper_state_str, GripperStepper::GripperState *out_gripper_state);

    /**
     * Gets the desired stepper motor position for a given gripper state.
     */
    static int get_desired_step_position(GripperState state);
    
    /**
     * Converts millimeters to stepper motor steps.
     */
    static int mm_to_steps(int mm);
    
    /**
     * Converts stepper motor steps to millimeters.
     */
    static int steps_to_mm(int steps);

    static const int steps_per_revolution; // Steps per full rotation (half-step mode)
    static const int speed;                // Target speed for normal operation [steps/sec]
    static const int max_speed;            // Maximum speed for rapid movements [steps/sec]
    static const int acceleration;         // Acceleration rate [steps/sec²]
    static const float transmission_ratio; // Distance traveled per rotation [mm/rotation]
    static const int plate_distance_open;  // Plate separation when fully open [mm]
    static const int plate_distance_large; // Plate separation for large raspberry detection [mm]
    static const int plate_distance_small; // Plate separation for small raspberry detection [mm]
    static const int plate_distance_limit; // Plate separation at limit switch [mm]
};

#endif
