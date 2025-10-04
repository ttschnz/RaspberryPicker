#ifndef RASPBERRY_PICKER_GRIPPER_H
#define RASPBERRY_PICKER_GRIPPER_H

#include <Stepper.h>

#include "Gripper/ColorSensor.h"
#include "Gripper/PressureSensor.h"
#include "Gripper/ResistanceSensor.h"
#include "Gripper/PlateStepper.h"
#include "InterfaceSender.h"

typedef struct {
    int color_sensor_pin;
    int pressure_sensor_pin;
    int resistance_sensor_pin;
    int stepper_motor_pins[4];
} GripperPinout;

class GripperController{
    public:
        /**
        *
        */
        GripperController(GripperPinout *pinout,InterfaceSender *interface);
    private:

        ~GripperController() {
            delete plate_stepper;  // prevent memory leak
        }

        Stepper *plate_stepper;
        InterfaceSender *interface;

};

#endif
