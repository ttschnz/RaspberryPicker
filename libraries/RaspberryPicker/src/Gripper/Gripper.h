#ifndef RASPBERRY_PICKER_GRIPPER_H
#define RASPBERRY_PICKER_GRIPPER_H

#include <AccelStepper.h>

#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "ColorSensor.h"
#include "PressureSensor.h"
#include "GripperStepper.h"

typedef struct {
    ColorSensor::Pinout color_sensor_pinout;
    int pressure_sensor_pins[2];
    int stepper_motor_pins[4];
} GripperPinout;




class GripperController{
    public:
        /**
        * Initialises the controller for the gripper subcontroller. This also
        * initialises all subcomponents of this controller, such as sensors and
        * motors.
        * Call this during setup.
        */
        GripperController(GripperPinout *pinout,InterfaceMaster *interface);

        /**
         * sets the gripper to a certain state:
         * - Open: calculates how many steps need to be done in order to be open
         * - Close: closes the gripper in small intervals and measures each time
         *          if we are allready touching the berry or not. It then
         *          evaluates the size of the berry we are touching and returns
         *          the more likely size.
         */
        GripperStepper::RaspberrySize set_gripper(GripperStepper::GripperState desired_gripper_state);

        /**
         * returns true if the color sensor thinks the berry is ripe.
         * returns false if not
         */
        bool is_ripe();

        ColorSensor *color_sensor;
    private:

        ~GripperController() {
            delete plate_stepper;  // prevent memory leak
        }
        GripperStepper::GripperState gripper_state;
        float plate_distance;
        AccelStepper *plate_stepper;
        InterfaceMaster *interface;
        PressureSensor *pressure_sensor;
};

#endif
