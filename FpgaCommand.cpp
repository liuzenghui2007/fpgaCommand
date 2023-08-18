#include "FpgaCommand.h"
#include "common.h"

FpgaCommand::FpgaCommand(uint32_t commandNumber, uint32_t registerAddress, const std::vector<uint32_t>& commandContent)
    : commandNumber(commandNumber), registerAddress(registerAddress), commandContent(commandContent) {

    commandLength = static_cast<uint16_t>(2 + 2 + 4 +4 + 4 * commandContent.size() + 4);
    commandData.resize(commandLength);
    fillCommandData();
    showCommand();
}

const std::vector<uint8_t>& FpgaCommand::getCommandData() const {
    return commandData;
}

void FpgaCommand::fillCommandData() {
    int currentIndex = 0;

    // 命令头，逐个字节直接写入
    commandData[currentIndex++] = 0xFF;
    commandData[currentIndex++] = 0xAA;

    // 命令长度，换序
    write_uint16(&commandData[currentIndex], commandLength);
    currentIndex += 2;

    // 命令编号，换序
    write_uint32(&commandData[currentIndex], commandNumber);
    currentIndex += 4;

    // 寄存器地址， 换序
    write_uint32(&commandData[currentIndex],  registerAddress);
    currentIndex += 4;

    // 命令内容, 这里要逆序
    // 默认从低位处理，这里要从高位处理
    for (auto it = commandContent.rbegin(); it != commandContent.rend(); ++it) {
//        std::cout << *it << std::endl;
        write_uint32(&commandData[currentIndex],  *it);
        currentIndex += 4;
    }


    // 命令尾
    commandData[currentIndex++] = 0x00;
    commandData[currentIndex++] = 0x00;
    commandData[currentIndex++] = 0x55;
    commandData[currentIndex++] = 0xAA;
}


// 在 FpgaCommand.cpp 中实现方法
void FpgaCommand::showCommand() const {
    for (const auto& byte : commandData) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}
