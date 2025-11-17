#ifndef RASPBERRY_PICKER_BASKET_DOOR_H
#define RASPBERRY_PICKER_BASKET_DOOR_H

class BasketDoor
{
    public:
        /**
        * Door State:
        * - OPEN: The door is open
        * - CLOSED: The door is closed
        */
        enum class DoorState {
            OPEN,
            CLOSED,
        };

        static const char* serialize_door_state(BasketDoor::DoorState door_state);
        static bool deserialize_door_state(String door_state_str, BasketDoor::DoorState* out_door_state);

        static const int closed_pos;        // position at which the both containers are closed  [deg]
        static const int open_pos;          // position at which the berry container is open [deg]
        static const int max_fill;          // how many raspberries can we fit?
        static const int delay_ms;          // how long do we need to wait between opening and closing the door? (i.e. how slow are the servos?) [ms]
};

#endif
