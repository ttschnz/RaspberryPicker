#ifndef RASPBERRY_PICKER_BASKET_DOOR_H
#define RASPBERRY_PICKER_BASKET_DOOR_H

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

static const char* door_state_strings[] = {
    "OPEN_SMALL",
    "OPEN_LARGE",
    "CLOSED"
};

const char* door_state_to_str(DoorState door_State);

class DoorServoValues
{
    public:
        static const int closed_pos;        // position at which the both containers are closed  [deg]
        static const int open_large_pos;    // position at which the large berry container is open [deg]
        static const int open_small_pos;    // position at which the small berry container is open [deg]
        static const int speed;             // measured angular speed to predict position [deg/s]
};

#endif
