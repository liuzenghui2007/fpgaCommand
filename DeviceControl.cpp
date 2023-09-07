#include <iostream>
#include <iomanip>
#include <string>
#include <cstddef> // For std::byte

#include "DeviceControl.h"

DeviceControl::DeviceControl() {
    libusb_init(&context);
    buffer_size =  P1000FrameSize * P1000FrameCount;
    bufferData = new unsigned char[buffer_size];
}

DeviceControl::~DeviceControl() {
    if (handle) {
        libusb_close(handle);
    }
    libusb_exit(context);
    // 释放 buffer 内存
    delete[] bufferData;
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
    int transferred;
    int transferredTimes = 0;

    // 用于计算数据传输速率的变量
    std::atomic<std::size_t> totalTransferredData;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::vector<cv::Mat> matrices;  // 存储多个矩阵的容器

    while (deviceControl->isReading) {
        int result = libusb_bulk_transfer(deviceControl->handle, deviceControl->endpoint_data, deviceControl->bufferData, deviceControl->buffer_size, &(deviceControl->transferred_data), 1000);
        if (result == 0) {
            totalTransferredData += deviceControl->transferred_data;
            transferredTimes++;
        } else {
            std::cerr << "Failed to read data: " << libusb_error_name(result) << std::endl;
        }

        // 计算已经经过的时间
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        if (elapsedTime >= 1) { // 每秒输出一次
            // 计算传输速率（MB/s）并输出
            double transferRateMBps = static_cast<double>(totalTransferredData) / (1024 * 1024);
            std::cout << std::dec << "Transfer rate for the last second: " << transferRateMBps << " MB/s, " << transferredTimes << " times/s" <<std::endl;

            // 重置已传输的数据总量和起始时间
            totalTransferredData = 0;
            transferredTimes = 0;
            startTime = currentTime;

            if (deviceControl->transferred_data > 8) {
                // 打印前四个字节的十六进制值
                for (int i = 0; i < 8; i++) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(deviceControl->bufferData[i]) << " ";
                }

                std::cout << std::endl;
            }
        }
        // 如果readCacheReserved为false且transferred_data小于buffer_size，则退出函数
        if (deviceControl->transferred_data < deviceControl->buffer_size) {
            deviceControl->isReading = false;
            break;
        } else {
            // 创建一个 cv::Mat, 将数据从缓冲区复制到 cv::Mat 对象中
            cv::Mat mat(P1000FrameCount, P1000FrameSize, CV_8U);
            memcpy(mat.data, deviceControl->bufferData, P1000FrameCount * P1000FrameSize);
        }
    }
}

void DeviceControl::StartReadThread() {
    isReading = true;
    // 启动异步读取线程
    // 传递函数指针
     std::thread readerThread(&DeviceControl::ReadDataAsync, this);
    // 启动异步读取线程
    //    std::thread readerThread([this, &stopFlag]() {
    //        this->ReadDataAsync(this, stopFlag);
    //    });

    std::cout << "Press Enter to stop reading..." << std::endl;

    // 清空标准输入缓冲区
    while (std::cin.peek() != EOF) {
        std::cin.get();
    }

    // 循环等待空格键的按下
    while (true) {
        int key = getchar();
        if (key == ' ') {
            break;  // 如果按下空格键，退出循环
        }
    }

    // 停止数据采集
    isReading = false;

    readerThread.join();
}

void DeviceControl::StartRead() {
    isReading = true;
    this->ReadDataAsync(this);

}

