#ifndef RASPBERRY_PICKER_GRIPPER_H
#define RASPBERRY_PICKER_GRIPPER_H

#include <Stepper.h>

#include "../InterfaceMaster.h"
#include "../Basket/Basket.h"

#include "ColorSensor.h"
#include "PressureSensor.h"
#include "GrabberStepper.h"

typedef struct {
    ColorSensor::Pinout color_sensor_pinout;
    int pressure_sensor_pins[2];
    int stepper_motor_pins[4];
} GripperPinout;

enum RaspberrySize{
    LARGE,
    SMALL
};

static const char* raspberry_size_strings[] = {
    "LARGE",
    "SMALL"
};

const char* raspberry_size_to_str(RaspberrySize raspberry_size);
bool str_to_raspberry_size(String raspberry_size_str, RaspberrySize* out_raspberry_size);

class GripperController{
    public:
        /**
        *
        */
        GripperController(GripperPinout *pinout,InterfaceMaster *interface);

        RaspberrySize set_grabber(GrabberState desired_grabber_state);
        bool is_ripe();
        bool is_touching();
    private:

        ~GripperController() {
            delete plate_stepper;  // prevent memory leak
        }

        Stepper *plate_stepper;
        InterfaceMaster *interface;
        PressureSensor *pressure_sensor;
        ColorSensor *color_sensor;
};

#endif
