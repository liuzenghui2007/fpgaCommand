#include <iostream>
#include <iomanip>
#include <string>
#include <cstddef> // For std::byte

#include "DeviceControl.h"

std::atomic<std::size_t> DeviceControl::totalTransferredData = 0;
unsigned char** DeviceControl::bufferData = new unsigned char*[4];
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


    int result = libusb_bulk_transfer(handle, endpoint_out, const_cast<uint8_t*>(command), length, &transferred_data, 10);
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

void DeviceControl::TransferCallback(struct libusb_transfer* transfer) {
    // 这是异步传输的回调函数，可以在这里处理传输完成后的操作
    // 你可以将需要的处理逻辑放在这里，例如计算传输速率和处理数据
    // 注意：在回调函数中使用std::cout等输出函数时要小心，
    // 最好使用线程安全的方式，或者将数据存储在共享的数据结构中，由主线程来输出
    int i = transfer->num_iso_packets;
    int index = (int)(DeviceControl::bufferData[i][12])*256*256*256 +(int)(DeviceControl::bufferData[i][13])*256*256+ (int)(DeviceControl::bufferData[i][14])*256 + (int)(DeviceControl::bufferData[i][15]);
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
    {
        // 采集无异常，重新提交transfer
//        DeviceControl::totalTransferredData += transfer->actual_length;
//        std::cout << std::dec << "received=" << transfer->actual_length << " total=" << DeviceControl::totalTransferredData << "index="<<index <<std::endl;
        if(index - DeviceControl::totalTransferredData == 1024){
            DeviceControl::totalTransferredData = index;
        }else{
            std::cout <<std::dec<<index <<"error"<<DeviceControl::totalTransferredData<<std::endl;
            DeviceControl::totalTransferredData = index;
        }
        libusb_submit_transfer(transfer);
    } else if (transfer->status == LIBUSB_TRANSFER_CANCELLED)
    {
        // Transfer was cancelled, do nothing
        std::cout << "canceled" << std::endl;
    }
    else
    {
        // Handle transfer error
        std::cout << "Transfer error: " << libusb_error_name(transfer->status) << std::endl;
    }
}

void DeviceControl::ReadDataOnce(DeviceControl* deviceControl) {
    auto startTime = std::chrono::steady_clock::now();
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

        if (elapsedTime >= 500)
            break;

        int transferResult = libusb_bulk_transfer(deviceControl->handle, deviceControl->endpoint_data, deviceControl->bufferDataAll, deviceControl->TRANSFER_SIZE, &(deviceControl->transferred_data), 10);
        if (transferResult == 0 && deviceControl->transferred_data != 0)
        {
            std::cout << "received data length=" << deviceControl->transferred_data << std::endl;
        }

        // 等待一段时间，避免while循环过于频繁占用CPU资源
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "read cache finished" << std::endl;

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
        libusb_fill_bulk_transfer(transfers[i], deviceControl->handle, deviceControl->endpoint_data, deviceControl->bufferData[i], deviceControl->TRANSFER_SIZE, TransferCallback, nullptr, 0);
    }

    for (int i = 0; i < TRANSFER_NUM; i++)
    {
        int ret = libusb_submit_transfer(transfers[i]);
        if (ret != LIBUSB_SUCCESS)
        {
            std::cout << "Initial Submit transfer error: " << libusb_error_name(ret);
        }
    }
    // adc使能
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

    for (auto & transfer : transfers) {
        libusb_cancel_transfer(transfer);
        libusb_free_transfer(transfer);
    }
}

void DeviceControl::StartReadThread() {
    // 启动异步读取线程
    // 传递函数指针
//    std::thread readerThread(&DeviceControl::ReadDataAsync, this);
//    readerThread.join();
}

void DeviceControl::StartRead() {

    this->ReadDataOnce(this);

}

