#ifndef RASPBERRY_PICKER_INTERFACE_SENDER_H
#define RASPBERRY_PICKER_INTERFACE_SENDER_H

typedef struct {
    int rx;
    int tx;
} InterfacePinout;

class InterfaceSender {
    public:
        InterfaceSender(InterfacePinout* pinout);
};

#endif
