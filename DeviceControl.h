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
    bool host2Device(const uint8_t* command, int length);
    bool device2Host(uint8_t* buffer, int length);

private:
    const int interface_number = 0;
    const int vid = 0x0ff8;
    const int pid = 0x00ff;


    int OUT_ENDPOINT = 0x01;
    int IN_ENDPOINT = 0x81;
    int DATA_ENDPOINT = 0x82;

    libusb_device_handle* handle;
    libusb_context* context;

    void fillUint32ToBytes(uint32_t value, uint8_t* bytes, int index);
};

#endif // DEVICE_CONTROL_H
