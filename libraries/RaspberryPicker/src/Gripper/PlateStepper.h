#ifndef RASPBERRY_PICKER_GRIPPER_PLATE_H
#define RASPBERRY_PICKER_GRIPPER_PLATE_H

class PlateStepperMotorValues
{
    public:
        static const int steps_per_revolution; // amount of steps per revolution
        static const int speed;                // speed (RPM) to use while driving
        static const float transmission_ratio; // how many mm per second do the plates move per rpm? [30/pi*mm/s]
        static const int plate_distance;       // how many mm do we want the plates to be apart from each other in the open state?
        static const int measuring_interval;   // how many mm do we move until we check if we have pressure?
};

#endif
