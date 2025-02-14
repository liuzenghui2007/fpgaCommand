#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <libusb.h>
#include <vector>
#include <thread> // 读取数据流，单独启动线程
#include <atomic> // For atomic flag
#include <chrono> // For time measurement
#include <cstdlib> // For system()
#include <fstream>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>


struct TransferInfo
{
    std::chrono::high_resolution_clock::time_point submitTime;
    std::chrono::high_resolution_clock::time_point submitTimeLast;
    std::chrono::high_resolution_clock::time_point receiveTime;
    std::chrono::steady_clock::duration  transferDuration;
    std::chrono::steady_clock::duration  callbackDuration;
};
// 发送命令command在外部构造
// 接收buffer是类的私有属性
class DeviceControl {
public:
    DeviceControl();
    ~DeviceControl();

    // 控制部分
    bool deviceOpen();
    bool sendCmd(const uint8_t* command, int commandLength); //0x01
    int receiveData();     //0x81
    unsigned char* getBuffer();
    void StartRead();
    static void ReadDataOnce(DeviceControl* deviceControl);
    // 数据流部分
    // 新增的常量成员

//    static const uint16_t P2560FrameSize = 1312;
//    static const uint16_t P2560FrameCount = 8;
    static std::atomic<std::size_t> totalTransferredData;
    static void TransferCallback(struct libusb_transfer* transfer);
    static void ReadDataAsync(DeviceControl* deviceControl);
    static void SaveLog(const std::string& log);
    static void ProcessData(uint8_t* buffer, std::size_t length);
    static std::ofstream logFile;
    static std::ofstream datFile;
    static bool exitRequested;
    // 数据流部分
    bool isReading = false;
    const static int TRANSFER_NUM = 4;
    static const uint16_t FrameSize = 1312;
    static const uint16_t FrameCount = 512;
    const static int TRANSFER_SIZE = FrameCount * FrameSize;
    const size_t total_buffer_size = TRANSFER_NUM * TRANSFER_SIZE ;
    // 转后数据
    static const uint16_t ChannelSize = 640;
    static const size_t total_data_size = FrameCount * ChannelSize ;
    static float *dataFloatAll;

    // 总buffer和分buffer指向同一片地址区域
    unsigned char *bufferDataAll = new unsigned char[total_buffer_size];
    static unsigned char** bufferData;
    // 必须在类外初始化
    static std::chrono::high_resolution_clock::time_point transferStartTime;
    static TransferInfo transferInfoList[TRANSFER_NUM];

private:
    // 硬件描述
    const int interface_number = 0;
    const int vid = 0x3701;
    const int pid = 0x0a00;
    unsigned char endpoint_out = 0x01;
    unsigned char endpoint_in = 0x81;
    unsigned char endpoint_data = 0x82;
    // libusb公共
    libusb_device_handle* handle = nullptr ; // nullptr
    libusb_context* context = nullptr;      // nullptr

    // 控制部分，n有 1 2 4 40 129，其中4只有读用到，40和129只有写用到
    // 16 + n * 4, n是返回内容需要的uint32的数量
    // n=1, 20个字节 = 12 + 1 * 4 + 2 + 2， 中间1个uint32_t, 也就是13-16字节是返回内容， 17-18字节是出错信息
    // n=4, 32个字节 = 12 + 4 * 4 + 2 + 2， 中间8个uint32_t, 也就是13-28字节是返回内容， 29-30字节是出错信息
    // n=40, 176个字节 = 12 + 40 * 4 + 2 + 2， 中间40个uint32_t, 写模式用
    // n=129, 只有写模式
    // 接收数据 的缓冲区、最大数量、实际传输字节，只用32个字节就可以。读取返回最大长度是这个
    int length = 32;
    unsigned char buffer[32];
    int transferred = 0;   // 实际接收数据长度

    int transferred_data = 0;  // 实际接收数据长度
    // 用于计算数据传输速率的变量
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

#endif // DEVICE_CONTROL_H
