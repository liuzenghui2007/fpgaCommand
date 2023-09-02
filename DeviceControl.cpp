#include <iostream>
#include <iomanip>
#include <string>
#include <cstddef> // For std::byte

#include "DeviceControl.h"

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
    // devicesList();
    // opendevice
    // release interface
    // claim interface
    handle = libusb_open_device_with_vid_pid(context, vid, pid);
    if (handle == nullptr) {
        std::cerr << "Device not found or cannot be opened." << std::endl;
        return false;
    }
    libusb_release_interface(handle, interface_number);
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

bool DeviceControl::sendCmd(const uint8_t* command, int length) {
    // 这里length是传入的，不是private成员
    if (!handle) {
        std::cerr << "Device not opened." << std::endl;
        return false;
    }


    std::cout << std::dec << "Sent Length: " << length << std::endl;
    std::cout << "Send command: ";
    for (int i = 0; i < length; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(command[i]) << " ";
    }
    std::cout << std::endl;

    int transferred;
    int result = libusb_bulk_transfer(handle, endpoint_out, const_cast<uint8_t*>(command), length, &transferred, 10);
    if (result != LIBUSB_SUCCESS) {
        std::cerr << "Error sending command to the device. " << result << std::endl;
        return false;
    }
    // 结束
    return true;
}

int DeviceControl::receiveData() {
    if (!handle) {
        std::cerr << "Device not opened." << std::endl;
        return false;
    }

    int result = libusb_bulk_transfer(handle, endpoint_in, (unsigned char*)&buffer, length, &transferred, 100);
    if (result != LIBUSB_SUCCESS) {
        std::cerr << "Error reading data from the device. " << libusb_strerror(static_cast<libusb_error>(result))  << std::endl;
        return false;
    }
    std::cout << std::dec << "Received: " << transferred << std::endl;
    std::cout << "Received data: ";
    for (int i = 0; i < transferred; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(buffer[i]) << " ";
    }
    std::cout << std::endl;

    return transferred;
}

unsigned char* DeviceControl::getBuffer() {
    return buffer;
}

// 静态函数
// 将 ReadDataAsync 修改为静态成员函数，那么在函数内部无法访问非静态成员变量。
// 为了解决这个问题，你可以将需要访问的成员变量传递给 ReadDataAsync 函数。
// 在 DeviceControl.cpp 中实现 ReadDataAsync
void DeviceControl::ReadDataAsync(DeviceControl* deviceControl) {
    unsigned char buffer[DeviceControl::TRANSFER_SIZE];
    int transferred;

    while (deviceControl->isReading) {
        int result = libusb_bulk_transfer(deviceControl->handle, deviceControl->endpoint_data, buffer, DeviceControl::TRANSFER_SIZE, &(deviceControl->transferred_data), 1000);
        if (result == 0) {
            std::cout << "Read " << deviceControl->transferred_data << " bytes of data: ";
            for (int i = 0; i < deviceControl->transferred_data; i++) {
                std::cout << static_cast<int>(buffer[i]) << " ";
            }
            std::cout << std::endl;
        } else {
            std::cerr << "Failed to read data: " << libusb_error_name(result) << std::endl;
        }
    }
}

void DeviceControl::StartReadThread() {
    isReading = true;
    // 启动异步读取线程
    // 线程函数需要是静态成员函数或者全局函数
    std::thread readerThread(ReadDataAsync);

    std::cout << "Press Enter to stop reading..." << std::endl;
    std::cin.get();

    isReading = false;
    readerThread.join();
}

