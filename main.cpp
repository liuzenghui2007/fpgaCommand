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



    std::vector<uint32_t> initData(1, 0);
    CommandContent cmdContent(initData);

    FpgaCommand cmd(1, RegisterEnum::READ_ASIC_STATUS_REG_32BIT, cmdContent.getData());



    // 使用 host2Device 函数发送命令数据
    DeviceControl deviceCtrl;
    deviceCtrl.deviceOpen();



    cmd.showCommand();
    if (!deviceCtrl.host2Device(cmd.getCommandData().data(), cmd.getCommandData().size())) {
        std::cerr << "Failed to send command." << std::endl;
    }

    return 0;
}
