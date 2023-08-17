#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <bitset>

#include "CommandRegister.h"
#include "CommandContent.h"
#include "FpgaCommand.h"
#include "DeviceControl.h"

#define STRINGIFY(x) #x

int main() {
//    std::vector<uint32_t> initData = {0, 0}; // 初始化数据，可根据需要修改大小



    std::vector<uint32_t> initData(1, 0);
    CommandContent cmdContent(initData);

    //    cmdContent.setBitValue(0, true);
    //    cmdContent.setBitsRangeFromTo(1,7, 0b1111111);
    //    这里输出，目前是低32在前
    //    cmdContent.hexShow();
    //    cmdContent.binaryShow();

    FpgaCommand cmd(1, RegisterEnum::READ_ASIC_STATUS_REG_32BIT, cmdContent.getData());

    std::cout << cmd.getCommandData().size() << std::endl;



    // 获取命令数据
    const std::vector<uint8_t>& commandData = cmd.getCommandData();

    // 使用 host2Device 函数发送命令数据
    DeviceControl deviceCtrl;
    deviceCtrl.deviceOpen();
    if (!deviceCtrl.host2Device(commandData.data(), commandData.size())) {
        std::cerr << "Failed to send command." << std::endl;
    }

    return 0;
}
