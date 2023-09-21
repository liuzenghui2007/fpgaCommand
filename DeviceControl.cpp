#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "DeviceControl.h"

std::ofstream DeviceControl::logFile;
std::ofstream DeviceControl::datFile;
std::chrono::high_resolution_clock::time_point DeviceControl::transferStartTime = std::chrono::high_resolution_clock::now();
std::atomic<std::size_t> DeviceControl::totalTransferredData = 0;
unsigned char** DeviceControl::bufferData = new unsigned char*[DeviceControl::TRANSFER_NUM];
TransferInfo DeviceControl::transferInfoList[DeviceControl::TRANSFER_NUM];

DeviceControl::DeviceControl() {
    libusb_init(&context);
    for (int i = 0; i < 4; i++) {
        DeviceControl::transferInfoList[i] = {
                std::chrono::high_resolution_clock::now(),
                std::chrono::high_resolution_clock::now(),
                std::chrono::high_resolution_clock::now(),
                std::chrono::steady_clock::duration(),
                std::chrono::steady_clock::duration()
        };
    }
    // Generate a filename with a timestamp
    std::ostringstream logFileName;
    std::ostringstream datFileName;
    auto now = std::chrono::system_clock::now();
    auto timePoint = std::chrono::system_clock::to_time_t(now);
    logFileName << "log_" << std::put_time(std::localtime(&timePoint), "%Y-%m-%d_%H-%M-%S") << ".txt";
    datFileName << "log_" << std::put_time(std::localtime(&timePoint), "%Y-%m-%d_%H-%M-%S") << ".dat";
    // Open the log file with the generated filename
    logFile.open(logFileName.str());

    datFile.open(datFileName.str(),std::ofstream::binary);
}

DeviceControl::~DeviceControl() {
    if (handle) {
        libusb_close(handle);
    }
    libusb_exit(context);
    // 释放 buffer 内存
    delete[] bufferDataAll;
    logFile.close();
}


bool DeviceControl::deviceOpen() {
    // release interface
    // claim interface
    handle = libusb_open_device_with_vid_pid(context, vid, pid);
    if (handle == nullptr) {
        std::cerr << "Device not found or cannot be opened." << std::endl;
        return false;
    }
    libusb_release_interface(handle, interface_number);
    int ret = libusb_claim_interface(handle, interface_number);
    // 获取设备描述符
//    libusb_device **devList = NULL;
//    ssize_t count = libusb_get_device_list(nullptr, &devList);
//    libusb_device *dev = devList[1];
//    struct libusb_device_descriptor deviceDescriptor;
//    memset(&deviceDescriptor, 0, sizeof(struct libusb_device_descriptor));
//    libusb_get_device_descriptor(dev, &deviceDescriptor);
//    // 获取活跃的配置描述符的 bConfigurationValue
//    uint8_t active_bConfigurationValue = 0;
//    struct libusb_config_descriptor *configDescriptor = NULL;
//    libusb_get_active_config_descriptor(dev, &configDescriptor);
//    if(configDescriptor){
//        active_bConfigurationValue = configDescriptor->bConfigurationValue;
//        libusb_free_config_descriptor(configDescriptor);
//        configDescriptor = NULL;
//    }
//    static char* speedString[] = {
//            "未知速度",
//            "低速(1500KBit/s)",
//            "全速(12000KBit/s)",
//            "高速(480000KBit/s)",
//            "超高速(5000000KBit/s)",
//            "超超速(10000000KBit/s)"
//    };
//
//    uint8_t bus   = libusb_get_bus_number(dev);
//    uint8_t addr  = libusb_get_device_address(dev);
//    int     speed = libusb_get_device_speed(dev);
//    printf("/dev/bus/usb/%03u/%03u  %s\n", bus, addr, speedString[speed]);
//    // 获取所有配置描述符
//    uint8_t configIndex;
//    for(configIndex=0; configIndex<deviceDescriptor.bNumConfigurations; configIndex++){
//        configDescriptor = NULL;
//        libusb_get_config_descriptor(dev, configIndex, &configDescriptor);
//        if(configDescriptor){
//            // 活跃的配置每个字段前面加一个*号
//            if(configDescriptor->bConfigurationValue == active_bConfigurationValue){
//                printf("      >*configDescriptor->bLength             = %u\n",       configDescriptor->bLength);
//                printf("       *configDescriptor->bDescriptorType     = %02X\n",     configDescriptor->bDescriptorType);
//                printf("       *configDescriptor->wTotalLength        = %u\n",       configDescriptor->wTotalLength);
//                printf("       *configDescriptor->bNumInterfaces      = %u\n",       configDescriptor->bNumInterfaces);
//                printf("       *configDescriptor->bConfigurationValue = %02X\n",     configDescriptor->bConfigurationValue);
//                printf("       *configDescriptor->iConfiguration      = %u\n",       configDescriptor->iConfiguration);
//                printf("       *configDescriptor->bmAttributes        = %02X\n",     configDescriptor->bmAttributes);
//                printf("       *configDescriptor->MaxPower            = %u\n", configDescriptor->MaxPower);
//            }else{
//                printf("      > configDescriptor->bLength             = %u\n",       configDescriptor->bLength);
//                printf("        configDescriptor->bDescriptorType     = %02X\n",     configDescriptor->bDescriptorType);
//                printf("        configDescriptor->wTotalLength        = %u\n",       configDescriptor->wTotalLength);
//                printf("        configDescriptor->bNumInterfaces      = %u\n",       configDescriptor->bNumInterfaces);
//                printf("        configDescriptor->bConfigurationValue = %02X\n",     configDescriptor->bConfigurationValue);
//                printf("        configDescriptor->iConfiguration      = %u\n",       configDescriptor->iConfiguration);
//                printf("        configDescriptor->bmAttributes        = %02X\n",     configDescriptor->bmAttributes);
//                printf("        configDescriptor->MaxPower            = %u\n", configDescriptor->MaxPower);
//            }
//
//            // 打印所有接口描述符
//            int numAltsetting;
//            int altsettingIndex;
//            uint8_t endpointIndex;
//            uint8_t interfaceIndex;
//            struct libusb_endpoint_descriptor *endpointDescriptor;
//            struct libusb_interface_descriptor *interfaceDescriptor;
//            for(interfaceIndex=0; interfaceIndex<configDescriptor->bNumInterfaces; interfaceIndex++){
//                numAltsetting = configDescriptor->interface[interfaceIndex].num_altsetting;
//                printf("          > interface[%u]:numberOfAltsetting = %d\n", interfaceIndex, numAltsetting);
//                for(altsettingIndex=0; altsettingIndex<numAltsetting; altsettingIndex++){
//                    interfaceDescriptor = (libusb_interface_descriptor*)&(configDescriptor->interface[interfaceIndex].altsetting[altsettingIndex]);
//                    printf("              > interfaceDescriptor->bLength            = %u\n",   interfaceDescriptor->bLength);
//                    printf("                interfaceDescriptor->bDescriptorType    = %02X\n", interfaceDescriptor->bDescriptorType);
//                    printf("                interfaceDescriptor->bInterfaceNumber   = %u\n",   interfaceDescriptor->bInterfaceNumber);
//                    printf("                interfaceDescriptor->bAlternateSetting  = %u\n",   interfaceDescriptor->bAlternateSetting);
//                    printf("                interfaceDescriptor->bNumEndpoints      = %u\n",   interfaceDescriptor->bNumEndpoints);
//                    printf("                interfaceDescriptor->bInterfaceClass    = %02X\n", interfaceDescriptor->bInterfaceClass);
//                    printf("                interfaceDescriptor->bInterfaceSubClass = %02X\n", interfaceDescriptor->bInterfaceSubClass);
//                    printf("                interfaceDescriptor->bInterfaceProtocol = %02X\n", interfaceDescriptor->bInterfaceProtocol);
//                    printf("                interfaceDescriptor->iInterface         = %u\n",   interfaceDescriptor->iInterface);
//
//                    // 打印所有端点描述符
//                    if((interfaceDescriptor->bNumEndpoints) && (interfaceDescriptor->endpoint)){
//                        for(endpointIndex=0; endpointIndex<interfaceDescriptor->bNumEndpoints; endpointIndex++){
//                            endpointDescriptor = (libusb_endpoint_descriptor*)&(interfaceDescriptor->endpoint[endpointIndex]);
//                            printf("                  > endpointDescriptor->bLength             = %u\n",   endpointDescriptor->bLength);
//                            printf("                    endpointDescriptor->bDescriptorType     = %02X\n", endpointDescriptor->bDescriptorType);
//                            printf("                    endpointDescriptor->bEndpointAddress    = %02X\n", endpointDescriptor->bEndpointAddress);
//                            printf("                    endpointDescriptor->bmAttributes        = %02X\n", endpointDescriptor->bmAttributes);
//                            printf("                    endpointDescriptor->wMaxPacketSize      = %u\n",   endpointDescriptor->wMaxPacketSize);
//                            printf("                    endpointDescriptor->bInterval           = %u\n",   endpointDescriptor->bInterval);
//                        }
//                    }
//                }
//            }
//
//            libusb_free_config_descriptor(configDescriptor);
//            configDescriptor = NULL;
//        }
//    }
//    libusb_open(dev, &handle);
//    if (handle == nullptr) {
//        std::cerr << "Device not found or cannot be opened." << std::endl;
//        return false;
//    }
//    libusb_release_interface(handle, interface_number);
//    int ret = libusb_claim_interface(handle, interface_number);
    if (ret != LIBUSB_SUCCESS)
    {
        std::cerr << "Failed to claim USB interface: " << libusb_strerror(static_cast<libusb_error>(ret)) << std::endl;
        libusb_close(handle);
        libusb_exit(nullptr);
        return false;
    }

    return true;
}

bool DeviceControl::sendCmd(const uint8_t* command, int commandLength) {
    // 这里length是传入的，不是private成员
    if (!handle) {
        std::cerr << "Device not opened." << std::endl;
        return false;
    }


    std::cout << std::dec << "Sent Length: " << commandLength << std::endl;
    std::cout << "Send command: ";
    for (int i = 0; i < commandLength; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(command[i]) << " ";
    }
    std::cout << std::endl;


    int result = libusb_bulk_transfer(handle, endpoint_out, const_cast<uint8_t*>(command), commandLength, &transferred_data, 10);
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

// Add the savelog method to save log information to the file
void DeviceControl::SaveLog(const std::string& log) {
    logFile << log << std::endl;
}

std::string subtractAndFormatTime(
        const std::chrono::time_point<std::chrono::steady_clock>& timePoint1,
        const std::chrono::time_point<std::chrono::steady_clock>& timePoint2) {

    // 相减，得到时间间隔
    auto time_difference = std::chrono::duration_cast<std::chrono::seconds>(timePoint1 - timePoint2);

    // 计算小时、分钟和秒
    int hours = time_difference.count() / 3600;
    int minutes = (time_difference.count() % 3600) / 60;
    int seconds = time_difference.count() % 60;

    // 格式化为字符串
    std::stringstream formatted_time;
    formatted_time << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;

    return formatted_time.str();
}


void DeviceControl::TransferCallback(struct libusb_transfer* transfer) {
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
    {
        unsigned int transfer_num = transfer->num_iso_packets;
        unsigned int frame_no = 0;
        for (int j = 12; j < 16; j++) {
            frame_no = (frame_no << 8) | DeviceControl::bufferData[transfer_num][j];
        }
//        // 从submit到receive是transfer时间
        DeviceControl::transferInfoList[transfer_num].receiveTime = std::chrono::steady_clock::now();
        DeviceControl::transferInfoList[transfer_num].transferDuration = DeviceControl::transferInfoList[transfer_num].receiveTime - DeviceControl::transferInfoList[transfer_num].submitTimeLast;


        if (transfer->actual_length != DeviceControl::TRANSFER_SIZE) {
            std::cout << std::dec << transfer->actual_length << " != " << DeviceControl::TRANSFER_SIZE << std::endl;
            return;
        }
        if(frame_no - DeviceControl::totalTransferredData == P1000FrameCount){
            DeviceControl::totalTransferredData = frame_no;
        }else{
            std::cout <<std::dec<< frame_no <<"error"<<DeviceControl::totalTransferredData<<std::endl;
            DeviceControl::totalTransferredData = frame_no;
        }
        // 从receive处理到完毕是callback时间
        DeviceControl::transferInfoList[transfer_num].callbackDuration = std::chrono::steady_clock::now() - DeviceControl::transferInfoList[transfer_num].receiveTime;

//
        std::cout << "transfer_num=" << transfer_num << " "
        << " frame_no=" << frame_no << " "
        << " actual_length=" <<  transfer->actual_length << " "
        << " check=" << frame_no % P1000FrameCount << " "
        << " transfer=" << std::chrono::duration_cast<std::chrono::milliseconds>(DeviceControl::transferInfoList[transfer_num].transferDuration).count() << " "
        << " callback=" << std::chrono::duration_cast<std::chrono::milliseconds>(DeviceControl::transferInfoList[transfer_num].callbackDuration).count() << " "
        << " elapsed=" << subtractAndFormatTime( std::chrono::steady_clock::now(), DeviceControl::transferStartTime)
        << std::endl;
        std::string logMessage = "transfer_num=" + std::to_string(transfer_num) + " "
                                 + " frame_no=" + std::to_string(frame_no) + " "
                                 + " actual_length=" + std::to_string(transfer->actual_length) + " "
                                 + " check=" + std::to_string(frame_no % 1024) + " "
                                 + " transfer=" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(DeviceControl::transferInfoList[transfer_num].transferDuration).count()) + " "
                                 + " callback=" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(DeviceControl::transferInfoList[transfer_num].callbackDuration).count());

        // Save the log message to the file
        DeviceControl::SaveLog(logMessage);

        if(frame_no % 1024!=0){
            datFile.close();
        }else {
            datFile.write((char*)DeviceControl::bufferData[transfer_num],transfer->actual_length);
        }

        libusb_submit_transfer(transfer);
        DeviceControl::transferInfoList[transfer_num].submitTimeLast = std::chrono::high_resolution_clock::now();

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
    DeviceControl::transferStartTime = std::chrono::high_resolution_clock::now();

    libusb_transfer* transfers[TRANSFER_NUM];
    // 初始化异步传输
    for (int i = 0; i < TRANSFER_NUM; ++i) {
        transfers[i] = libusb_alloc_transfer(0);
        transfers[i]->actual_length = 0;
        transfers[i]->num_iso_packets = i;
        DeviceControl::bufferData[i] = deviceControl->bufferDataAll + i * deviceControl->TRANSFER_SIZE;
        libusb_fill_bulk_transfer(transfers[i], deviceControl->handle, deviceControl->endpoint_data, deviceControl->bufferData[i], deviceControl->TRANSFER_SIZE, TransferCallback, nullptr, 0);
        int ret = libusb_submit_transfer(transfers[i]);
        if (ret != LIBUSB_SUCCESS)
        {
            std::cout << "Initial Submit transfer error: " << libusb_error_name(ret);
        }
        else {
            // 初始化 submitTime
            DeviceControl::transferInfoList[i].submitTimeLast = std::chrono::high_resolution_clock::now();
        }
    }

//    for (std::size_t i = 0; i < TRANSFER_NUM; ++i) {
//        auto &transfer = transfers[i];
//        int ret = libusb_submit_transfer(transfer);
//        if (ret != LIBUSB_SUCCESS)
//        {
//            std::cout << "Initial Submit transfer error: " << libusb_error_name(ret);
//        }
//        else {
//            // 初始化 submitTime
//            DeviceControl::transferInfoList[i].submitTimeLast = std::chrono::high_resolution_clock::now();
//        }
//    }
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

void DeviceControl::StartRead() {

    this->ReadDataOnce(this);

}
