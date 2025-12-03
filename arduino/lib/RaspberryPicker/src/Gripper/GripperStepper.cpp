/**
 * GripperStepper.cpp
 * 
 * Gripper stepper motor state management and unit conversion utilities.
 * Provides serialization/deserialization of gripper and raspberry size states,
 * and conversion functions between millimeters and stepper motor steps.
 */

#include <Arduino.h>
#include "GripperStepper.h"

// String representations of gripper states
static const char *gripper_state_strings[4] = {
    "OPEN",
    "CLOSED_SMALL",
    "CLOSED_LARGE",
    "CLOSED_LIMIT",
};

/**
 * Converts GripperState enum to string representation.
 * @param gripper_state Gripper state enum value
 * @return String representation of the gripper state
 */
const char *GripperStepper::serialize_gripper_state(GripperStepper::GripperState gripper_state)
{
    int idx = (int)gripper_state;
    return gripper_state_strings[idx];
}

/**
 * Converts string to GripperState enum.
 * @param gripper_state_str String representation of gripper state
 * @param out_gripper_state Pointer to store resulting GripperState enum
 * @return true if string matched a valid state, false otherwise
 */
bool GripperStepper::deserialize_gripper_state(String gripper_state_str, GripperStepper::GripperState *out_gripper_state)
{
    bool matched = true;
    if (gripper_state_str == "CLOSED_SMALL")
    {
        *out_gripper_state = GripperStepper::GripperState::CLOSED_SMALL;
    }
    else if (gripper_state_str == "CLOSED_LARGE")
    {
        *out_gripper_state = GripperStepper::GripperState::CLOSED_LARGE;
    }
    else if (gripper_state_str == "CLOSED_LIMIT")
    {
        *out_gripper_state = GripperStepper::GripperState::CLOSED_LIMIT;
    }
    else if (gripper_state_str == "OPEN")
    {
        *out_gripper_state = GripperStepper::GripperState::OPEN;
    }
    else
    {
        matched = false;
    }
    return matched;
}

/**
 * Converts millimeters to stepper motor steps.
 * Takes into account the transmission ratio and steps per revolution.
 * @param mm Distance in millimeters
 * @return Equivalent number of stepper motor steps
 */
int GripperStepper::mm_to_steps(int mm)
{
    double steps =
        (double)mm                                     // [mm]
        * (double)GripperStepper::steps_per_revolution // [steps/rotation]
        / (double)GripperStepper::transmission_ratio;  // [mm/rotation]
    return (int)steps;
}

/**
 * Converts stepper motor steps to millimeters.
 * @param steps Number of stepper motor steps
 * @return Equivalent distance in millimeters
 */
int GripperStepper::steps_to_mm(int steps)
{
    double mm =
        (double)steps                                   // [steps]
        * (double)GripperStepper::transmission_ratio    // [mm/rotation]
        / (double)GripperStepper::steps_per_revolution; // [steps/rotation]
    return (int)mm;
}

/**
 * Gets the desired stepper motor position for a given gripper state.
 * @param state Desired gripper state
 * @return Stepper motor position in steps
 */
int GripperStepper::get_desired_step_position(GripperState state)
{
    int desired_mm = 0;
    switch (state)
    {
    case GripperStepper::GripperState::OPEN:
        desired_mm = GripperStepper::plate_distance_open;
        break;
    case GripperStepper::GripperState::CLOSED_SMALL:
        desired_mm = GripperStepper::plate_distance_small;
        break;
    case GripperStepper::GripperState::CLOSED_LARGE:
        desired_mm = GripperStepper::plate_distance_large;
        break;
    case GripperStepper::GripperState::CLOSED_LIMIT:
        desired_mm = GripperStepper::plate_distance_limit;
        break;
    }

    int desired_steps = GripperStepper::mm_to_steps(desired_mm);

    return desired_steps;
}

// String representations of raspberry sizes
static const char *raspberry_size_strings[] = {
    "LARGE",
    "SMALL",
    "UNKNOWN",
};

/**
 * Converts RaspberrySize enum to string representation.
 * @param raspberry_size Raspberry size enum value
 * @return String representation of the raspberry size
 */
const char *GripperStepper::serialize_raspberry_size(GripperStepper::RaspberrySize raspberry_size)
{
    int idx = (int)raspberry_size;
    return raspberry_size_strings[idx];
}

/**
 * Converts string to RaspberrySize enum.
 * @param raspberry_size_str String representation of raspberry size
 * @param out_raspberry_size Pointer to store resulting RaspberrySize enum
 * @return true if string matched a valid size, false otherwise
 */
bool GripperStepper::deserialize_raspberry_size(String raspberry_size_str, GripperStepper::RaspberrySize *out_raspberry_size)
{
    bool matched = true;
    if (raspberry_size_str == "LARGE")
    {
        *out_raspberry_size = GripperStepper::RaspberrySize::LARGE;
    }
    else if (raspberry_size_str == "SMALL")
    {
        *out_raspberry_size = GripperStepper::RaspberrySize::SMALL;
    }
    else if (raspberry_size_str == "UNKNOWN")
    {
        *out_raspberry_size = GripperStepper::RaspberrySize::UNKNOWN;
    }
    else
    {
        matched = false;
    }
    return matched;
}
