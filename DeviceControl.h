#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <libusb.h>
#include <vector>
// 发送命令command在外部构造
// 接收buffer是类的私有属性
class DeviceControl {
public:
    DeviceControl();
    ~DeviceControl();

    void devicesList();
    bool deviceOpen();
    bool sendCmd(const uint8_t* command, int length);
    int receiveData();
    unsigned char* getBuffer();

private:
    const int interface_number = 0;
    const int vid = 0x0ff8;
//    const int pid = 0x00ff;
    const int pid = 0x20fc;


    unsigned char endpoint_out = 0x01;
    unsigned char endpoint_in = 0x81;
    unsigned char endpoint_data = 0x82;

    // 读取结果到buffer
    // 16 + n * 4, n是返回内容需要的uint32的数量
    // n=1, 20个字节 = 12 + 1 * 4 + 2 + 2， 中间1个uint32_t, 也就是13-16字节是返回内容， 17-18字节是出错信息
    // n=8, 48个字节 = 12 + 8 * 4 + 2 + 2， 中间8个uint32_t, 也就是13-48字节是返回内容， 49-50字节是出错信息
    // n=32, 144个字节 = 12 + 32 * 4 + 2 + 2， 中间32个uint32_t, 也就是13-140字节是返回内容， 141-142字节是出错信息
    // n=129, 只有写模式油
    // 接收数据 的缓冲区、最大数量、实际传输字节
    unsigned char buffer[144];
    int length = 144;
    int transferred;

    libusb_device_handle* handle;
    libusb_context* context;
};

#endif // DEVICE_CONTROL_H
