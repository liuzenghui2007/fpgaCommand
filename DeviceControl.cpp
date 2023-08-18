#include "DeviceControl.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <QString>
#include <QByteArray>

DeviceControl::DeviceControl() {
    libusb_init(&context);
}

DeviceControl::~DeviceControl() {
    if (handle) {
        libusb_close(handle);
    }
    libusb_exit(context);
}

void DeviceControl::devicesList() {
    libusb_device** deviceList;
    libusb_get_device_list(context, &deviceList);

    for (int i = 0; deviceList[i] != nullptr; ++i) {
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(deviceList[i], &desc);

        if (desc.idVendor == vid && desc.idProduct == pid) {
            std::cout << "Device found - VID: " << std::hex << desc.idVendor << " PID: " << desc.idProduct << std::endl;
        }
    }

    libusb_free_device_list(deviceList, 1);
}

bool DeviceControl::deviceOpen() {
//    devicesList();

    handle = libusb_open_device_with_vid_pid(context, vid, pid);
    if (handle == nullptr) {
        std::cerr << "Device not found or cannot be opened." << std::endl;
        return false;
    }
    int ret = libusb_claim_interface(handle, interface_number);
    if (ret != LIBUSB_SUCCESS)
    {
        std::cerr << "Failed to claim USB interface: " << libusb_strerror(static_cast<libusb_error>(ret)) << std::endl;
        libusb_close(handle);
        libusb_exit(NULL);
        return 1;
    }

    return true;
}

bool DeviceControl::host2Device(const uint8_t* command, int length) {
    if (!handle) {
        std::cerr << "Device not opened." << std::endl;
        return false;
    }

//    QString send = "FFAA0014000000018100000100000000000055AA";
//    QByteArray sendBuf = QByteArray::fromHex(send.toLatin1());

//    char getBuf[1000];
//    int acturalLenth = 0;

//    int r = libusb_bulk_transfer(handle, IN_ENDPOINT,  (unsigned char*)&getBuf, 20, &acturalLenth, 100);

//    if (r != LIBUSB_SUCCESS) {
//        std::cerr << "r0 Error sending command to the device. " << r << std::endl;
//        return false;
//    } else {
//        std::cout << "ok" << std::endl;
//    }

//    // 发送命令
//    r = libusb_bulk_transfer(handle, OUT_ENDPOINT,  (unsigned char*)sendBuf.data(), 20, &acturalLenth, 0);
//    // 接收命令
//    r = libusb_bulk_transfer(handle, IN_ENDPOINT,  (unsigned char*)&getBuf, 20, &acturalLenth, 0);


//    // 发送命令发fail
//    std::cout << "send command:" << std::endl;
//    for (size_t i = 0; i < length; ++i) {
//        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(command[i]) << " ";
//    }
//    std::cout << std::endl;

    int transferred;
    int result = libusb_bulk_transfer(handle, OUT_ENDPOINT, const_cast<uint8_t*>(command), length, &transferred, 10);
    if (result != LIBUSB_SUCCESS) {
        std::cerr << "Error sending command to the device. " << result << std::endl;
        return false;
    }
    // 结束
    return true;
}

bool DeviceControl::device2Host(uint8_t* getBuffer, int length) {
    if (!handle) {
        std::cerr << "Device not opened." << std::endl;
        return false;
    }

    int transferred;
    int result = libusb_bulk_transfer(handle, IN_ENDPOINT, (unsigned char*)&getBuffer, length, &transferred, 0);
    if (result != LIBUSB_SUCCESS) {
        std::cerr << "Error reading data from the device." << std::endl;
        return false;
    }

    return true;
}

void DeviceControl::fillUint32ToBytes(uint32_t value, uint8_t* bytes, int index) {
    for (int i = 0; i < 4; ++i) {
        bytes[index + i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
    }
}
