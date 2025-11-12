#ifndef RASPBERRY_PICKER_GRIPPER_LIMIT_SWITCH_H
#define RASPBERRY_PICKER_GRIPPER_LIMIT_SWITCH_H


class LimitSwitch{
    public:
        LimitSwitch(int pin);

        /**
         * returns true if the limit-switch's reading is HIGH
         */
        bool is_touching();
    private:
        int pin;
};

#endif
