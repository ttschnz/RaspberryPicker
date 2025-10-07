#ifndef RASPBERRY_PICKER_GRIPPER_H
#define RASPBERRY_PICKER_GRIPPER_H

#include <Stepper.h>

#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "ColorSensor.h"
#include "PressureSensor.h"
#include "GrabberStepper.h"

typedef struct {
    int color_sensor_pin;
    int pressure_sensor_pins[2];
    int resistance_sensor_pin;
    int stepper_motor_pins[4];
} GripperPinout;

enum RaspberrySize{
    LARGE,
    SMALL
};

class GripperController{
    public:
        /**
        *
        */
        GripperController(GripperPinout *pinout,InterfaceMaster *interface);

        RaspberrySize set_grabber(GrabberState desired_grabber_state);
        bool measure_color();
        bool is_touching();
    private:

        ~GripperController() {
            delete plate_stepper;  // prevent memory leak
        }

        Stepper *plate_stepper;
        InterfaceMaster *interface;
        PressureSensor *pressure_sensor;

};

#endif
