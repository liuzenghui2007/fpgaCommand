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

    cmdContent.fillContent(std::vector<uint32_t>(1, 0));
    cmdContent.setBitsRange(3, 2, 0b00); // 设置 3、4 bit 为00， 对应-1/3 mV/pA;
    cmdContent.setBitsRange(19,2, 0b11); // 设置19、20bit 为11， 对应3、4bit生效；

    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_CTRL_REG_32BIT, cmdContent.getData());
    // set gain

    return 0;
}
