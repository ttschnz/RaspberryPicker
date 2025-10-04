#ifndef RASPBERRY_INTERFACE_RECEIVER_H
#define RASPBERRY_INTERFACE_RECEIVER_H

typedef struct {
    int rx;
    int tx;
} InterfacePinout;

class InterfaceReceiver {
    public:
        InterfaceReceiver(InterfacePinout* pinout);
};

#endif
