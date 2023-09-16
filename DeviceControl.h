#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <libusb.h>
#include <vector>
#include <thread> // 读取数据流，单独启动线程
#include <atomic> // For atomic flag
#include <chrono> // For time measurement
#include <cstdlib> // For system()
#include <opencv2/opencv.hpp>

// 发送命令command在外部构造
// 接收buffer是类的私有属性
class DeviceControl {
public:
    DeviceControl();
    ~DeviceControl();

    // 控制部分
    void devicesList();
    bool deviceOpen();
    bool sendCmd(const uint8_t* command, int length); //0x01
    int receiveData();     //0x81
    unsigned char* getBuffer();
    void StartRead();
    static void ReadDataOnce(DeviceControl* deviceControl);
    // 数据流部分
    // 新增的常量成员
    static const uint16_t P1000FrameSize = 2080;
    static const uint16_t P1000FrameCount = 1024;
    static const uint16_t P2560FrameSize = 1312;
    static const uint16_t P2560FrameCount = 8;
    static std::atomic<std::size_t> totalTransferredData;
    static void TransferCallback(struct libusb_transfer* transfer);
    static void ReadDataAsync(DeviceControl* deviceControl);
    // 数据流部分
    bool isReading = false;
    const static int TRANSFER_NUM = 4;
    const int TRANSFER_SIZE = P1000FrameCount * P1000FrameSize;
    const size_t total_buffer_size = TRANSFER_NUM * TRANSFER_SIZE ;
    // 总buffer和分buffer指向同一片地址区域
//    unsigned char *bufferDataAll = new unsigned char[total_buffer_size];
    unsigned char *bufferDataAll = new unsigned char[total_buffer_size];
//    unsigned char* bufferData[TRANSFER_NUM];
    static unsigned char** bufferData;
    void StartReadThread();

private:
    // 硬件描述
    const int interface_number = 0;
    const int vid = 0x0ff8;
    const int pid = 0x00ff;
    // const int pid = 0x20fc;
    unsigned char endpoint_out = 0x01;
    unsigned char endpoint_in = 0x81;
    unsigned char endpoint_data = 0x82;
    // libusb公共
    libusb_device_handle* handle = nullptr ; // nullptr
    libusb_context* context = nullptr;      // nullptr

    // 控制部分
    // 16 + n * 4, n是返回内容需要的uint32的数量
    // n=1, 20个字节 = 12 + 1 * 4 + 2 + 2， 中间1个uint32_t, 也就是13-16字节是返回内容， 17-18字节是出错信息
    // n=8, 48个字节 = 12 + 8 * 4 + 2 + 2， 中间8个uint32_t, 也就是13-48字节是返回内容， 49-50字节是出错信息
    // n=32, 144个字节 = 12 + 32 * 4 + 2 + 2， 中间32个uint32_t, 也就是13-140字节是返回内容， 141-142字节是出错信息
    // n=129, 只有写模式油
    // 接收数据 的缓冲区、最大数量、实际传输字节
    unsigned char buffer[144];
    int length = 144;
    int transferred = 0;   // 实际接收数据长度



    int transferred_data = 0;  // 实际接收数据长度
    // 用于计算数据传输速率的变量
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

#endif // DEVICE_CONTROL_H
