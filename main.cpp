#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <bitset>

#include "CommandRegister.h"
#include "CommandContent.h"

#define STRINGIFY(x) #x

int main() {
//    std::vector<uint32_t> initData = {0, 0}; // 初始化数据，可根据需要修改大小
    std::vector<uint32_t> initData(2, 0);

    CommandContent cmd(initData);

//    设置第0位
    cmd.setBitValue(0, true);
//    设置1-7位
    cmd.setBitsRangeFromTo(1,7, 0b1111111);
//    设置,从32位开始，连续8位
    cmd.setBitsRange(32, 8, 0b11111111);
//    这里输出，目前是低32在前
    cmd.hexShow();
    cmd.binaryShow();

    RegisterEnum reg = RegisterEnum::READ_ADC_ACQ_TIME_CFG_REG_32BIT;
    std::cout << reg << std::endl;
    return 0;
}
