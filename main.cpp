#include <iostream>
#include <vector>

#include "CommandRegister.h"
#include "CommandContent.h"
#include "FpgaCommand.h"
#include "DeviceControl.h"

#include "testEigen.h"


// https://blog.csdn.net/zhouwu_linux/article/details/134413790
// https://blog.csdn.net/encourage2011/article/details/78760169
int main() {
    // write response
    // ff aa 00 14 00 00 00 01 01 00 00 02 00 00 00 00 00 00 55 aa
    // 12 是返回内容的起始字节
    // 12 20个字-4内容-4尾部

//    testEigen();
//    return 0;
    DeviceControl devCtrl;
    devCtrl.deviceOpen();

    int transferred = 0;


    // asic status
    // 初始化发送内容和返回内容，uint32_t * 1, 填充为0
    CommandContent cmdContent(std::vector<uint32_t>(1, 0));
    CommandContent resContent(std::vector<uint32_t>(1, 0));

    // 查询状态-读取
    std::cout << "读取状态" << std::endl;
    // 构造 发送 接收 解析 输出
    FpgaCommand cmd(1, RegisterEnum::READ_ASIC_STATUS_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    unsigned char* bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 16);
    int asicDet = resContent.getState(_asicStatus.ASIC_DET);
    int asicReady = resContent.getState(_asicStatus.ASIC_LOGIC_READY);
    std::cout << "asicAt=" << asicDet << std::endl;
    std::cout << "asicReady=" << asicReady << std::endl;
    resContent.showBin();


    // asic power
    std::cout << "asic power"  << std::endl;
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_POWER_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();

    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();

    // asic vcm
    std::cout << "asic vcm"  << std::endl;
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_VCM_EN_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();



    // asic vcom
    std::cout << "asic vcom"  << std::endl;
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_FC_VCOM_EN_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();



    // asic init
    // set sampling rate, uint is ns 1s=10^9ns
    std::cout << "asic set sampling rate to 5000"  << std::endl;
    uint32_t samplingRate = 5000;
    uint32_t samplingPeriod = 1000000000/(samplingRate * 12.5);
    cmdContent.reset(samplingPeriod);
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_SAMPLE_PERIOD_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();


    // set temperature to 38℃
    std::cout << "asic set temperature to 38 ℃"  << std::endl;
    uint8_t t = 38;
    uint32_t temperature = 65536 * (exp((1.0/(t+273.15)-1.0/298.15)*3380)+1)/(2+exp((1.0/(t+273.15)-1.0/298.15)*3380));
    cmdContent.reset(0);
    cmdContent.setBitsRange(0, 16, temperature);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_HEATER_TEMP_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();


    // set work mode, 8*32bit, 8bits / channel, P2560
    std::cout << "ADC每个周期内16个采样点采样通道号配置寄存器"  << std::endl;
    // command_send = "FFAA00200000000100000002000102030405060708090A0B0C0D0E0F000055AA";
    int totalADCRegs = 16; // 16个采样点
    int contentLength = 4; // *32bit，4*32bit数据内容区
    int channelLength = 8;  // bit，每个采样点8bit
    cmdContent.fillContent(std::vector<uint32_t>(contentLength, 0)); // 构造8*32
    for (int i= 0; i < totalADCRegs; i++ ) {
        int bitIndex = 32 * contentLength - channelLength * (i + 1);
        cmdContent.setBitsRange(bitIndex, channelLength, i);
    }
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_SAMPLE_CHANNEL_8X32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();


    // disable unblock 使用asic_ctrl,  value+mask都要设置
    std::cout << "asic set gain && disable unblock"  << std::endl;
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitsRange(3, 2, 0b00); // 设置 3、4 bit 为00， 对应-1/3 mV/pA;
    cmdContent.setBitsRange(16 + 3, 2, 0b11); // 设置19、20bit 为11， 对应3、4bit生效；
    cmdContent.setBitValue(0, 0);   // 设置0 bit为0，禁止疏通
    cmdContent.setBitValue(16 + 0, 1); // 设置对应掩码bit为1
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_CONTROL_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();


    // set protocol voltage fixed value 设定测序电压-恒定，100mV
    std::cout << "asic set vcom constant v"  << std::endl;
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitsRange(0, 15, 100);
    cmd.fillCommand(1, RegisterEnum::WRITE_FC_VCOM_OUTPUT_FIXED_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();

    // toggle protocol voltage mode fixed value 选定测序电压-恒定
    std::cout << "asic apply constant v"  << std::endl;
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitsRange(0, 1, 0b11); // 单次模式
    cmd.fillCommand(1, RegisterEnum::WRITE_FC_VCOM_MODE_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();

//    // adc enable
//    std::cout << "adc enable"  << std::endl;
//    cmdContent.fillContent(std::vector<uint32_t>(1,0));
//    cmdContent.setBitsRange(0, 1, 0b1); // adc 采样使能
//    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_ENABLE_32BIT, cmdContent.getData());
//    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
//    transferred = devCtrl.receiveData();
//    bufferPtr = devCtrl.getBuffer();
//    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
//    resContent.showBin();
//
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//
//    // adc disable
//    std::cout << "adc disable" << std::endl;
//    cmdContent.fillContent(std::vector<uint32_t>(1,0));
//    cmdContent.setBitsRange(0, 1, 0b0); // adc 采样使能
//    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_ENABLE_32BIT, cmdContent.getData());
//    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
//    transferred = devCtrl.receiveData();
//    bufferPtr = devCtrl.getBuffer();
//    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
//    resContent.showBin();
    // adc disable
    std::cout << "adc disable"  << std::endl;
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitsRange(0, 1, 0b0); // adc 取消使能
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_ENABLE_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();

    devCtrl.StartRead();

    // 查询状态-读取
    std::cout << "开始读取数据流*******************" << std::endl;
    std::cout << "读取状态, 清除溢出标志" << std::endl;
    // 构造 发送 接收 解析 输出
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmd.fillCommand(1, RegisterEnum::READ_ASIC_STATUS_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 16);
    asicDet = resContent.getState(_asicStatus.ASIC_DET);
    asicReady = resContent.getState(_asicStatus.ASIC_LOGIC_READY);
    std::cout << "asicAt=" << asicDet << std::endl;
    std::cout << "asicReady=" << asicReady << std::endl;
    resContent.showBin();

    std::thread readerThread(&DeviceControl::ReadDataAsync, &devCtrl);

    // adc enable
    std::cout << "adc enable"  << std::endl;
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitsRange(0, 1, 0b1); // adc 采样使能
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_ENABLE_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 12);
    resContent.showBin();

    readerThread.join();
    return 0;
}
