/**
 * Gripper.cpp
 * 
 * Gripper controller implementation for the Raspberry Picker.
 * Manages the gripper mechanism including:
 * - Stepper motor control for gripper plate movement
 * - Color sensor for ripeness detection
 * - Limit switches for position detection and raspberry contact
 * - Size detection based on gripper closure distance
 */

#include <AccelStepper.h>
#include <Arduino.h>

#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "Gripper.h"
#include "ColorSensor.h"
#include "GripperStepper.h"
#include "LimitSwitch.h"

// Color sensor timing constants
const int ColorSensor::measure_count = 10;     // Number of samples per measurement
const int ColorSensor::delay_probe = 10;       // Delay between samples (ms)
const int ColorSensor::delay_color = 200;      // Delay after LED activation (ms)

// Gripper stepper motor constants
const float GripperStepper::transmission_ratio = 1.5 * 18 * PI;  // Gear ratio * diameter * pi (mm/rotation)
const int GripperStepper::plate_distance_open = 65;              // Fully open position (mm)
const int GripperStepper::plate_distance_large = 30;             // Position for large raspberry detection (mm)
const int GripperStepper::plate_distance_small = 20;             // Position for small raspberry detection (mm)
const int GripperStepper::plate_distance_limit = 13;             // Limit switch position (mm)
const int GripperStepper::steps_per_revolution = 2048 * 2;       // Steps for one full rotation (half-step mode)
const int GripperStepper::speed = 200;                           // Target speed (steps/sec)
const int GripperStepper::max_speed = 1200;                      // Maximum speed (steps/sec)
const int GripperStepper::acceleration = 300;                    // Acceleration rate (steps/sec²)

// Gripper controller constants
const int GripperController::berry_size_threshold_mm = 21;  // Threshold between small and large raspberries (mm)
const int GripperController::picking_delay_ms = 10000;      // Maximum wait time for user to pick raspberry (ms)

/**
 * Constructor - initializes gripper controller with all sensors and motors.
 * @param pinout Pointer to GripperPinout structure with pin assignments
 * @param interface Pointer to InterfaceMaster for state communication
 */
GripperController::GripperController(GripperPinout *pinout, InterfaceMaster *interface)
{
    this->interface = interface;
    this->gripper_state = GripperStepper::GripperState::OPEN;
    
    // Initialize color sensor
    this->color_sensor = new ColorSensor(pinout->color_sensor_pinout);
    
    // Initialize limit switches
    this->limit_switch_pressure = new LimitSwitch(pinout->limit_switch_pressure_pin);
    this->limit_switch_zero = new LimitSwitch(pinout->limit_switch_zero_pin);
    
    // Initialize plate distance
    this->plate_distance = GripperStepper::plate_distance_open;
    
    // Initialize stepper motor with half-step 4-wire configuration
    this->plate_stepper = new AccelStepper(
        AccelStepper::HALF4WIRE,
        pinout->stepper_motor_pins[0],
        pinout->stepper_motor_pins[2],
        pinout->stepper_motor_pins[1],
        pinout->stepper_motor_pins[3]);

    // Configure stepper motor parameters
    this->plate_stepper->setCurrentPosition(
        GripperStepper::get_desired_step_position(GripperStepper::GripperState::OPEN));
    this->plate_stepper->setSpeed(GripperStepper::speed);
    this->plate_stepper->setMaxSpeed(GripperStepper::max_speed);
    this->plate_stepper->setAcceleration(GripperStepper::acceleration);
}

/**
 * Sets the gripper to the desired state and attempts to detect raspberry size.
 * 
 * Behavior varies by desired state:
 * - OPEN: Fully opens gripper
 * - CLOSED_LIMIT: Closes until limit switch or pressure plate activated
 * - CLOSED_SMALL/LARGE: Closes to specific position, detecting raspberry if pressure plate activated
 * 
 * @param desired_gripper_state Target gripper state
 * @return Detected raspberry size (LARGE, SMALL, or UNKNOWN)
 */
GripperStepper::RaspberrySize GripperController::set_gripper(GripperStepper::GripperState desired_gripper_state)
{
    int target_steps = GripperStepper::get_desired_step_position(desired_gripper_state);
    this->plate_stepper->setSpeed(GripperStepper::speed);
    switch (desired_gripper_state)
    {
    case GripperStepper::GripperState::OPEN:
    {
        // Open gripper fully - simple movement with periodic status updates
        this->plate_stepper->moveTo(target_steps);
        int i = 0;
        while (this->plate_stepper->isRunning())
        {
            this->plate_stepper->run();
            i++;
            if (i > 10000)
            {
                // Periodically update interface with current position
                i = 0;
                int current_position_step = this->plate_stepper->currentPosition();
                this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
                this->interface->send_state("gripper.plate_distance", this->plate_distance);
            }
        }

        // Update final position
        int current_position_step = this->plate_stepper->currentPosition();
        this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
        this->gripper_state = GripperStepper::GripperState::OPEN;
        this->interface->send_state("gripper.gripper_state", GripperStepper::serialize_gripper_state(this->gripper_state));
        this->interface->send_state("gripper.plate_distance", this->plate_distance);
        return GripperStepper::RaspberrySize::UNKNOWN;
    }
    break;
    case GripperStepper::GripperState::CLOSED_LIMIT:
    {
        // Close gripper until pressure plate or zero limit switch is triggered
        this->plate_stepper->moveTo(target_steps);
        bool limit_switch_zero = false;
        bool limit_switch_pressure = false;
        int i = 0;
        do
        {
            this->plate_stepper->run();

            i++;
            if (i > 10000)
            {
                // Periodically update interface with current position
                i = 0;
                int current_position_step = this->plate_stepper->currentPosition();
                this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
                this->interface->send_state("gripper.plate_distance", this->plate_distance);
            }

            // Check both limit switches
            limit_switch_pressure = this->limit_switch_pressure->is_touching();
            limit_switch_zero = this->limit_switch_zero->is_touching();

        } while (!limit_switch_pressure && !limit_switch_zero && this->plate_stepper->isRunning());

        if (limit_switch_pressure)
        {
            // Pressure plate activated - raspberry detected
            // Determine size based on current position
            GripperStepper::RaspberrySize size;
            GripperStepper::GripperState state;

            this->plate_stepper->setSpeed(0);  // Stop immediately

            int current_position_step = this->plate_stepper->currentPosition();
            int raspberry_width = GripperStepper::steps_to_mm(current_position_step);

            // Classify raspberry size based on width
            if (raspberry_width > GripperController::berry_size_threshold_mm)
            {
                size = GripperStepper::RaspberrySize::LARGE;
                state = GripperStepper::GripperState::CLOSED_LARGE;
            }
            else
            {
                size = GripperStepper::RaspberrySize::SMALL;
                state = GripperStepper::GripperState::CLOSED_SMALL;
            }

            this->gripper_state = state;
            this->interface->send_state("gripper.gripper_state",
                                        GripperStepper::serialize_gripper_state(state));

            return size;
        }
        else if (limit_switch_zero)
        {
            // Hit zero limit switch - recalibrate zero position
            this->plate_stepper->setSpeed(0);
            this->plate_stepper->setCurrentPosition(target_steps);
        }
        else
        {
            // Reached expected zero without triggering limit switch
            // Continue at low speed to find actual zero position
            Serial.println((String) + "closed without reaching limit switch. finding zero");
            this->plate_stepper->setSpeed(-GripperStepper::speed);
            while (!this->limit_switch_zero->is_touching())
            {
                this->plate_stepper->runSpeed();
            }
            this->plate_stepper->setCurrentPosition(target_steps);
        }
        return GripperStepper::RaspberrySize::UNKNOWN;
    }
    break;
    case GripperStepper::GripperState::CLOSED_LARGE:
    case GripperStepper::GripperState::CLOSED_SMALL:
    {
        // Close gripper to specific position (small or large)
        // Stop if pressure plate or zero limit switch is triggered
        this->plate_stepper->moveTo(target_steps);
        bool limit_switch_pressure = false;
        bool limit_switch_zero = false;
        int i = 0;
        do
        {
            this->plate_stepper->run();

            i++;
            if (i > 10000)
            {
                // Periodically update interface with current position
                i = 0;
                int current_position_step = this->plate_stepper->currentPosition();
                this->plate_distance = GripperStepper::steps_to_mm(current_position_step);
                this->interface->send_state("gripper.plate_distance", this->plate_distance);
            }

            // Check both limit switches
            limit_switch_pressure = this->limit_switch_pressure->is_touching();
            limit_switch_zero = this->limit_switch_zero->is_touching();

        } while (!limit_switch_pressure && !limit_switch_zero && this->plate_stepper->isRunning());

        GripperStepper::RaspberrySize size;
        GripperStepper::GripperState state;

        if (limit_switch_pressure)
        {
            // Pressure plate activated - raspberry detected during closure
            this->plate_stepper->setSpeed(0);

            int current_position_step = this->plate_stepper->currentPosition();
            int raspberry_width = GripperStepper::steps_to_mm(current_position_step);

            // Determine actual size based on where pressure was detected
            if (raspberry_width > GripperController::berry_size_threshold_mm)
            {
                size = GripperStepper::RaspberrySize::LARGE;
                state = GripperStepper::GripperState::CLOSED_LARGE;
            }
            else
            {
                size = GripperStepper::RaspberrySize::SMALL;
                state = GripperStepper::GripperState::CLOSED_SMALL;
            }
        }
        else
        {
            // No raspberry detected - reached target position or limit switch
            state = desired_gripper_state;
            size = GripperStepper::RaspberrySize::UNKNOWN;
        }

        this->gripper_state = state;
        this->interface->send_state("gripper.gripper_state",
                                    GripperStepper::serialize_gripper_state(state));

        return size;
    }
    break;
    }
    return GripperStepper::RaspberrySize::UNKNOWN;
}

/**
 * Determines if the currently held raspberry is ripe.
 * Uses color sensor to measure RGB values and applies logistic regression model.
 * 
 * @return true if raspberry is ripe, false if unripe
 */
bool GripperController::is_ripe()
{
    // Measure color values
    RAW_RGB color = this->color_sensor->measure_rgb_raw();

    // Send color measurements to interface
    this->interface->send_state("gripper.ripeness.r", color.r);
    this->interface->send_state("gripper.ripeness.g", color.g);
    this->interface->send_state("gripper.ripeness.b", color.b);
    this->interface->send_state("gripper.ripeness.noise", color.noise);

    // Get current plate distance for model input
    int current_position_step = this->plate_stepper->currentPosition();
    int plate_distance = GripperStepper::steps_to_mm(current_position_step);
    
    // Calculate ripeness probability using logistic regression model
    float ripeness_p = this->color_sensor->get_ripenesses_p(color, plate_distance);
    this->interface->send_state("gripper.raspberry_ripeness.p_ripe", ripeness_p);
    this->interface->send_state("gripper.raspberry_ripeness.p_unripe", 1 - ripeness_p);

    // TODO: Consider adding bias/threshold adjustment instead of 50/50 split
    return ripeness_p > 0.5;
}
