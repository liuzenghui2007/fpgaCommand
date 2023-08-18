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





    DeviceControl devCtrl;
    devCtrl.deviceOpen();


    // 读整体状态
    CommandContent cmdContent(std::vector<uint32_t>(1, 0));
    FpgaCommand cmd(255, RegisterEnum::READ_ASIC_STATUS_REG_32BIT, cmdContent.getData());

    if (!devCtrl.host2Device(cmd.getCommand().data(), cmd.getCommand().size())) {
        std::cerr << "Failed to send command." << std::endl;
    }

    // asic on
    // asic power
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_POWER_CTRL_REG_32BIT, cmdContent.getData());


    // asic vcm
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_VCM_EN_CTRL_REG_32BIT, cmdContent.getData());

    // asic vcom
    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_FC_VCOM_EN_CTRL_REG_32BIT, cmdContent.getData());

    // asic state
    cmdContent.reset(0);
    cmd.fillCommand(1, RegisterEnum::READ_ASIC_STATUS_REG_32BIT, cmdContent.getData());

    // asic init
    // set sampling rate
    // uint is ns 1s=10^9ns
    uint32_t samplingRate = 16000;
    uint32_t samplingPeriod = 1000000000/(samplingRate * 12.5);
    cmdContent.reset(samplingPeriod);
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_SAMPLE_PERIOD_CFG_REG_32BIT, cmdContent.getData());

    // set work mode
    int contentLength = 4;
    cmdContent.fillContent(std::vector<uint32_t>(contentLength, 0));
    for (int i= 0; i < 32; i++ ) {
        int bitIndex = 32 * contentLength - 4 * (i + 1);
        cmdContent.setBitsRange(bitIndex, 4, i);
    }
    // cmdContent.binaryShow();
    cmd.fillCommand(1, RegisterEnum::WRITE_ADC_SAMPLE_PERIOD_CFG_REG_32BIT, cmdContent.getData());

    // set gain 使用asic_ctrl
    cmdContent.fillContent(std::vector<uint32_t>(1, 0));
    cmdContent.setBitsRange(3, 2, 0b00); // 设置 3、4 bit 为00， 对应-1/3 mV/pA;
    cmdContent.setBitsRange(16 + 3, 2, 0b11); // 设置19、20bit 为11， 对应3、4bit生效；
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_CTRL_REG_32BIT, cmdContent.getData());

    // disable unblock 使用asic_ctrl
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitValue(0, 0);   // 设置0 bit为0，禁止疏通
    cmdContent.setBitValue(16 + 0, 1); // 设置对应掩码bit为1
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_CTRL_REG_32BIT, cmdContent.getData());

    // set protocol voltage fixed value 设定测序电压-恒定
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmd.fillCommand(1, RegisterEnum::WRITE_FC_VCOM_FIXED_OUTPUT_REG_32BIT, cmdContent.getData());

    // toggle protocol voltage mode fixed value 选定测序电压-恒定
    cmdContent.fillContent(std::vector<uint32_t>(1,0));
    cmdContent.setBitsRange(0, 1, 0b11); // 单次模式
    cmd.fillCommand(1, RegisterEnum::WRITE_FC_VCOM_MODE_CONTROL_REG_32BIT, cmdContent.getData());
    return 0;
}
