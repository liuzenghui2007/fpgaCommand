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
    cmdContent.reset(0);

    return 0;
}
