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

    cmdContent.reset(0);
    cmdContent.setBitValue(0, 1);
    cmd.fillCommand(1, RegisterEnum::WRITE_ASIC_POWER_CTRL_REG_32BIT, cmdContent.getData());



    return 0;
}
