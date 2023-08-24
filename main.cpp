#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <bitset>

#include "CommandRegister.h"
#include "CommandContent.h"
#include "FpgaCommand.h"
#include "DeviceControl.h"



int main() {
//    std::vector<uint32_t> initData = {0, 0}; // 初始化数据，可根据需要修改大小

    // Print the assigned values
//    for (int i = 0; i < 32; ++i) {
//        std::cout << "Channel " << i + 1 << ": low = " << AdcSamplePeriod.ch[i].low << ", high = " << AdcSamplePeriod.ch[i].high << std::endl;
//    }


    DeviceControl devCtrl;
    devCtrl.deviceOpen();

    int ret = 0;
    int transferred = 0;


    // asic status
    CommandContent cmdContent(std::vector<uint32_t>(1, 0));
    CommandContent resContent(std::vector<uint32_t>(1, 0));
    FpgaCommand cmd(1, RegisterEnum::READ_ASIC_STATUS_32BIT, cmdContent.getData());

    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    unsigned char* bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 16);
    int asicDet = resContent.getState(_asicStatus.ASIC_DET);
    int asicReady = resContent.getState(_asicStatus.ASIC_LOGIC_READY);
    std::cout << "asicAt=" << asicDet << std::endl;
    std::cout << "asicReady=" << asicReady << std::endl;



    // asic on
    // asic power
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_POWER_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());

    cmd.fillCommand(1, RegisterEnum::READ_ASIC_STATUS_32BIT, cmdContent.getData());
    devCtrl.sendCmd(cmd.getCommand().data(), cmd.getCommand().size());
    transferred = devCtrl.receiveData();
    bufferPtr = devCtrl.getBuffer();
    resContent.fillFromBuffer(bufferPtr + 12, transferred - 16);
    int asicPower = resContent.getState(_AsicPower.powerEnable);
    std::cout << "asicPower" << asicPower <<std::endl;

    return 0;

    // asic vcm
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_VCM_EN_32BIT, cmdContent.getData());

    // asic vcom
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_FC_VCOM_EN_32BIT, cmdContent.getData());

    // asic state
    cmdContent.reset(0);
    cmd.fillCommand(1, RegisterEnum::READ_ASIC_STATUS_32BIT, cmdContent.getData());

    // asic init
    // set sampling rate
    // uint is ns 1s=10^9ns
    uint32_t samplingRate = 16000;
    uint32_t samplingPeriod = 1000000000/(samplingRate * 12.5);
    cmdContent.reset(samplingPeriod);
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_SAMPLE_PERIOD_32BIT, cmdContent.getData());

    // set work mode
    // 工作模式用4个32bit设置
    int contentLength = 4;
    cmdContent.fillContent(std::vector<uint32_t>(contentLength, 0));
    for (int i= 0; i < 32; i++ ) {
        int bitIndex = 32 * contentLength - 4 * (i + 1);
        cmdContent.setBitsRange(bitIndex, 4, i);
    }
    // cmdContent.binaryShow();
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_SAMPLE_PERIOD_32BIT, cmdContent.getData());

    // set gain 使用asic_ctrl
    cmdContent.fillContent(std::vector<uint32_t>(1, 0));
    cmdContent.setBitsRange(3, 2, 0b00); // 设置 3、4 bit 为00， 对应-1/3 mV/pA;
    cmdContent.setBitsRange(16 + 3, 2, 0b11); // 设置19、20bit 为11， 对应3、4bit生效；
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_MODE_32BIT, cmdContent.getData());

    // disable unblock 使用asic_ctrl
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitValue(0, 0);   // 设置0 bit为0，禁止疏通
    cmdContent.setBitValue(16 + 0, 1); // 设置对应掩码bit为1
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_MODE_32BIT, cmdContent.getData());

    // set protocol voltage fixed value 设定测序电压-恒定
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmd.fillCommand(1, RegisterEnum::WRITE_FC_VCOM_OUTPUT_FIXED_32BIT, cmdContent.getData());

    // toggle protocol voltage mode fixed value 选定测序电压-恒定
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitsRange(0, 1, 0b11); // 单次模式
    cmd.fillCommand(1, RegisterEnum::WRITE_FC_VCOM_MODE_32BIT, cmdContent.getData());
    return 0;
}
