/**
 * Gripper.h
 * 
 * Gripper controller for the Raspberry Picker.
 * Manages the gripper mechanism including:
 * - Stepper motor control for gripper plate movement
 * - Color sensor for ripeness detection
 * - Limit switches for position detection and raspberry contact
 * - Size detection based on gripper closure distance
 */

#ifndef RASPBERRY_PICKER_GRIPPER_H
#define RASPBERRY_PICKER_GRIPPER_H

#include <AccelStepper.h>

#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "LimitSwitch.h"
#include "ColorSensor.h"
#include "GripperStepper.h"

/**
 * GripperPinout structure - defines pin assignments for gripper components.
 */
typedef struct
{
    ColorSensor::Pinout color_sensor_pinout;  // Pins for color sensor (RGB LEDs and LDR)
    int stepper_motor_pins[4];                // Pins for stepper motor (4-wire configuration)
    int limit_switch_zero_pin;                // Pin for zero position limit switch
    int limit_switch_pressure_pin;            // Pin for pressure detection limit switch
} GripperPinout;

/**
 * GripperController class - controls gripper mechanism and sensors.
 * Manages stepper motor, color sensor, and limit switches for raspberry detection and handling.
 */
class GripperController
{
public:
    /**
     * Constructor - initializes the gripper controller with all sensors and motors.
     * Initializes all subcomponents: color sensor, limit switches, and stepper motor.
     * Call this during setup.
     */
    GripperController(GripperPinout *pinout, InterfaceMaster *interface);

    /**
     * Sets the gripper to a specified state and detects raspberry size.
     * 
     * Behavior varies by desired state:
     * - OPEN: Fully opens gripper
     * - CLOSED_LIMIT: Closes until limit switch or pressure plate activated
     * - CLOSED_SMALL/LARGE: Closes to specific position, detecting raspberry if present
     * 
     * @param desired_gripper_state Target gripper state
     * @return Detected raspberry size (LARGE, SMALL, or UNKNOWN)
     */
    GripperStepper::RaspberrySize set_gripper(GripperStepper::GripperState desired_gripper_state);

    /**
     * Determines if the currently held raspberry is ripe.
     * Uses color sensor to measure RGB values and applies ripeness detection model.
     * @return true if the raspberry is ripe, false if unripe
     */
    bool is_ripe();

    ColorSensor *color_sensor;                      // Pointer to color sensor
    GripperStepper::GripperState gripper_state;     // Current gripper state
    float plate_distance;                           // Current distance between gripper plates [mm]
    AccelStepper *plate_stepper;                    // Pointer to stepper motor controller
    InterfaceMaster *interface;                     // Pointer to interface master
    LimitSwitch *limit_switch_zero;                 // Pointer to zero position limit switch
    LimitSwitch *limit_switch_pressure;             // Pointer to pressure detection limit switch

    const static int berry_size_threshold_mm;       // Threshold between small and large raspberries [mm]
    const static int picking_delay_ms;              // Maximum wait time for user to pick raspberry [ms]

private:
    /**
     * Destructor - prevents memory leak by cleaning up stepper motor.
     */
    ~GripperController()
    {
        delete plate_stepper; // prevent memory leak
    }
};

#endif
