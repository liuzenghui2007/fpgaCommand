#include "FpgaCommand.h"

FpgaCommand::FpgaCommand(uint32_t commandNumber, uint32_t registerAddress, const std::vector<uint32_t>& commandContent)
    : commandNumber(commandNumber), registerAddress(registerAddress), commandContent(commandContent) {
//    std::cout << "content size = " << commandContent.size() * 4 << std::endl;
    commandLength = static_cast<uint16_t>(2 + 2 + 4 +4 + 4 * commandContent.size() + 4); // 4 bytes per commandContent
    commandData.resize(commandLength);
    fillCommandData();
    showCommand();
}

const std::vector<uint8_t>& FpgaCommand::getCommandData() const {
    return commandData;
}

void FpgaCommand::fillCommandData() {
    int currentIndex = 0;

    // 命令头，正确
    commandData[currentIndex++] = 0xFF;
    commandData[currentIndex++] = 0xAA;

    // 命令长度，低位在前
    commandData[currentIndex++] = static_cast<uint8_t>((commandLength >> 8) & 0xFF);
    commandData[currentIndex++] = static_cast<uint8_t>(commandLength & 0xFF);


    // 命令编号
    fillUint32ToBytes(commandNumber, commandData, currentIndex);
    currentIndex += 4;

    //  寄存器地址
    fillUint32ToBytes(registerAddress, commandData, currentIndex);
    currentIndex += 4;

    // 命令内容, 这里要逆序
    // 默认从低位处理，这里要从高位处理
    for (auto it = commandContent.rbegin(); it != commandContent.rend(); ++it) {
//        std::cout << *it << std::endl;
        fillUint32ToBytes(*it, commandData, currentIndex);
        currentIndex += 4;
    }


    // 命令尾
    fillUint32ToBytes(0x000055AA, commandData, currentIndex);
}

// 这里应该是uint32 的反字节顺序， 正字节序是+i， 反字节序是+3-i
void FpgaCommand::fillUint32ToBytes(uint32_t value, std::vector<uint8_t>& bytes, int index) {
    for (int i = 0; i < 4; ++i) {
        bytes[index + 3 - i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
    }
}

// 在 FpgaCommand.cpp 中实现方法
void FpgaCommand::showCommand() const {
    for (const auto& byte : commandData) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}
