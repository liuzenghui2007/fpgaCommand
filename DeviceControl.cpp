#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "DeviceControl.h"

std::ofstream DeviceControl::logFile;
std::ofstream DeviceControl::datFile;
bool DeviceControl::exitRequested = false;
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
        const std::chrono::time_point<std::chrono::high_resolution_clock>& timePoint1,
        const std::chrono::time_point<std::chrono::high_resolution_clock>& timePoint2) {

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
        DeviceControl::transferInfoList[transfer_num].receiveTime = std::chrono::high_resolution_clock ::now();
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
        DeviceControl::transferInfoList[transfer_num].callbackDuration = std::chrono::high_resolution_clock ::now() - DeviceControl::transferInfoList[transfer_num].receiveTime;

        int check = frame_no % P1000FrameCount;
        int transferTime = std::chrono::duration_cast<std::chrono::milliseconds>(DeviceControl::transferInfoList[transfer_num].transferDuration).count();
        int callbackTime = std::chrono::duration_cast<std::chrono::milliseconds>(DeviceControl::transferInfoList[transfer_num].callbackDuration).count();
        std::string elapsetdTime = subtractAndFormatTime( std::chrono::high_resolution_clock::now(), DeviceControl::transferStartTime);

        std::cout << "transfer_num=" << transfer_num << " "
        << " frame_no=" << frame_no << " "
        << " actual_length=" <<  transfer->actual_length << " "
        << " check=" << check << " "
        << " transfer=" << transferTime  << " "
        << " callback=" << callbackTime << " "
        << " elapsed=" << elapsetdTime
        << std::endl;
        std::string logMessage = "transfer_num=" + std::to_string(transfer_num) + " "
                                 + " frame_no=" + std::to_string(frame_no) + " "
                                 + " actual_length=" + std::to_string(transfer->actual_length) + " "
                                 + " check=" + std::to_string(check) + " "
                                 + " transfer=" + std::to_string(transferTime) + " "
                                 + " callback=" + std::to_string(callbackTime) + " "
                                 + " elasped=" + elapsetdTime;

        // Save the log message to the file
        DeviceControl::SaveLog(logMessage);

        datFile.write((char*)DeviceControl::bufferData[transfer_num],transfer->actual_length);

        if (check != 0) {
            exitRequested = true;  // Set the exit flag
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
    }

    for (std::size_t i = 0; i < TRANSFER_NUM; ++i) {
        auto &transfer = transfers[i];
        int ret = libusb_submit_transfer(transfer);
        if (ret != LIBUSB_SUCCESS)
        {
            std::cout << "Initial Submit transfer error: " << libusb_error_name(ret);
        }
        else {
            // 初始化 submitTime
            DeviceControl::transferInfoList[i].submitTimeLast = std::chrono::high_resolution_clock::now();
        }
    }

    while (!exitRequested)
    {
        int ret = libusb_handle_events(nullptr);
        if (ret < 0)
        {
            std::cout << "Handle events error: " << libusb_error_name(ret);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

//    while (true) {
//        int ret = libusb_handle_events_completed(nullptr, nullptr);
//        if (ret < 0) {
//            std::cerr << "Handle events error: " << libusb_error_name(ret) << std::endl;
//            break;
//        }
//        // 在处理事件之间加入一些延时，以避免过于频繁地检查事件, ms
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    }

    for (auto & transfer : transfers) {
        libusb_cancel_transfer(transfer);
        libusb_free_transfer(transfer);
    }
}

void DeviceControl::StartRead() {

    this->ReadDataOnce(this);

}
