#include <iostream>
#include <iomanip>
#include <string>
#include <cstddef> // For std::byte

#include "DeviceControl.h"

std::atomic<std::size_t> DeviceControl::totalTransferredData = 0;

DeviceControl::DeviceControl() {
    libusb_init(&context);
}

DeviceControl::~DeviceControl() {
    if (handle) {
        libusb_close(handle);
    }
    libusb_exit(context);
    // 释放 buffer 内存
    delete[] bufferDataAll;
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

void LIBUSB_CALL DeviceControl::TransferCallback(struct libusb_transfer* transfer) {
    // 这是异步传输的回调函数，可以在这里处理传输完成后的操作
    // 你可以将需要的处理逻辑放在这里，例如计算传输速率和处理数据
    // 注意：在回调函数中使用std::cout等输出函数时要小心，最好使用线程安全的方式，或者将数据存储在共享的数据结构中，由主线程来输出
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
    {
        // 采集无异常，重新提交transfer
        DeviceControl::totalTransferredData += transfer->actual_length;
        std::cout << "received=" << transfer->actual_length << " total=" << DeviceControl::totalTransferredData << std::endl;
        libusb_submit_transfer(transfer);
    } else if (transfer->status == LIBUSB_TRANSFER_CANCELLED)
    {
        // Transfer was cancelled, do nothing
    }
    else
    {
        // Handle transfer error
        std::cout << "Transfer error: " << libusb_error_name(transfer->status);
    }
}

// 静态函数
// 将 ReadDataAsync 修改为静态成员函数，那么在函数内部无法访问非静态成员变量。
// 为了解决这个问题，你可以将需要访问的成员变量传递给 ReadDataAsync 函数。
// 在 DeviceControl.cpp 中实现 ReadDataAsync
void DeviceControl::ReadDataAsync(DeviceControl* deviceControl) {

    libusb_transfer* transfers[TRANSFER_NUM];
    // 初始化异步传输
    for (int i = 0; i < TRANSFER_NUM; ++i) {
        transfers[i] = libusb_alloc_transfer(0);
        transfers[i]->actual_length = 0;
        transfers[i]->num_iso_packets = i;
        deviceControl->bufferData[i] = deviceControl->bufferDataAll + i * deviceControl->TRANSFER_SIZE;
        libusb_fill_bulk_transfer(transfers[i], deviceControl->handle, deviceControl->endpoint_data, deviceControl->bufferData[i], deviceControl->TRANSFER_SIZE, TransferCallback, nullptr, 1000);
        libusb_submit_transfer(transfers[i]);
    }

    while (true)
    {
        int ret = libusb_handle_events(nullptr);
        if (ret < 0)
        {
            std::cout << "Handle events error: " << libusb_error_name(ret);
            break;
        }
        // usleep(1);
    }

    // 取消和释放异步传输
    std::cout << "结束";
    for (int i = 0; i < TRANSFER_NUM; ++i) {
        libusb_cancel_transfer(transfers[i]);
        libusb_free_transfer(transfers[i]);
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

