/**
 * LimitSwitch.h
 * 
 * Simple limit switch interface.
 * Provides digital input reading for detecting physical contact/touch.
 * Used for gripper zero position detection and raspberry pressure sensing.
 */

#ifndef RASPBERRY_PICKER_GRIPPER_LIMIT_SWITCH_H
#define RASPBERRY_PICKER_GRIPPER_LIMIT_SWITCH_H

/**
 * LimitSwitch class - interface for digital limit switches.
 * Reads digital input to detect switch activation.
 */
class LimitSwitch
{
public:
    /**
     * Constructor - initializes limit switch on specified pin.
     * @param pin Digital pin number for the limit switch
     */
    LimitSwitch(int pin);

    /**
     * Checks if the limit switch is currently activated.
     * @return true if switch is pressed/touching (HIGH signal), false otherwise
     */
    bool is_touching();

private:
    int pin;  // Digital pin number for limit switch
};

#endif
