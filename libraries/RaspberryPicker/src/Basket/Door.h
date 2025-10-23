#ifndef RASPBERRY_PICKER_BASKET_DOOR_H
#define RASPBERRY_PICKER_BASKET_DOOR_H



static const char* door_state_strings[] = {
    "OPEN_SMALL",
    "OPEN_LARGE",
    "CLOSED"
};


class BasketDoor
{
    public:
        /**
        * Door State:
        * - OPEN_LARGE: The door for large berries is open
        * - OPEN_SMALL: The door for small berries is open
        * - CLOSED: The door is closed
        */
        enum class DoorState {
            OPEN_SMALL,
            OPEN_LARGE,
            CLOSED,
        };

        static const char* serialize_door_state(BasketDoor::DoorState door_state);
        static bool deserialize_door_state(String door_state_str, BasketDoor::DoorState* out_door_state);

        static const int closed_pos;        // position at which the both containers are closed  [deg]
        static const int open_large_pos;    // position at which the large berry container is open [deg]
        static const int open_small_pos;    // position at which the small berry container is open [deg]
        static const int speed;             // measured angular speed to predict position [deg/s]
        static const int max_fill;          // how many raspberries can we fit?
};

#endif
