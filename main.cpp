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

    DeviceControl deviceCtrl;
    deviceCtrl.deviceOpen();

    std::vector<uint32_t> initData(2, 0);
    CommandContent cmdContent(initData);

    cmdContent.setBitValue(0, true);
    cmdContent.setBitsRangeFromTo(1,7, 0b1111111);

    //    这里输出，目前是低32在前
    //    cmdContent.hexShow();
    //    cmdContent.binaryShow();

    FpgaCommand fpgaCommand(1, RegisterEnum::READ_ADC_ACQ_TIME_CFG_REG_32BIT, cmdContent.getData());



    return 0;
}
