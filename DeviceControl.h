#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <libusb.h>
#include <vector>

class DeviceControl {
public:
    DeviceControl();
    ~DeviceControl();

    void devicesList();
    bool deviceOpen();
    bool sendCmd(const uint8_t* command, int length);
    bool receiveData();

private:
    const int interface_number = 0;
    const int vid = 0x0ff8;
    const int pid = 0x00ff;


    int endpoint_out = 0x01;
    int endpoint_in = 0x81;
    int endpoint_data = 0x82;

    char buffer[32];
    int length = 32;

    libusb_device_handle* handle;
    libusb_context* context;

    void fillUint32ToBytes(uint32_t value, uint8_t* bytes, int index);
};

#endif // DEVICE_CONTROL_H
