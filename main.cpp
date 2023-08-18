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




    CommandContent cmdContent(std::vector<uint32_t>(1, 0));

    FpgaCommand cmd(255, RegisterEnum::READ_ASIC_STATUS_REG_32BIT, cmdContent.getData());
    cmd.showCommand();

    return 0;
}
