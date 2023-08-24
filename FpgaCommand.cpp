#include "FpgaCommand.h"
#include "common.h"

FpgaCommand::FpgaCommand(uint32_t commandNumber, uint32_t registerAddress, const std::vector<uint32_t>& commandContent)
    : commandNumber(commandNumber), registerAddress(registerAddress), commandContent(commandContent) {

    fillCommand(commandNumber,  registerAddress, commandContent);
}

const std::vector<uint8_t>& FpgaCommand::getCommand() const {
    return command;
}

void FpgaCommand::fillCommand(uint32_t commandNumber, uint32_t registerAddress, const std::vector<uint32_t>& commandContent) {

    commandLength = static_cast<uint16_t>(16 + 4 * commandContent.size());
    command.resize(commandLength);

    int currentIndex = 0;

    // 命令头，逐个字节直接写入
    command[currentIndex++] = 0xFF;
    command[currentIndex++] = 0xAA;

    // 命令长度，换序
    write_uint16(&command[currentIndex], commandLength);
    currentIndex += 2;

    // 命令编号，换序
    write_uint32(&command[currentIndex], commandNumber);
    currentIndex += 4;

    // 寄存器地址， 换序
    write_uint32(&command[currentIndex],  registerAddress);
    currentIndex += 4;

    // 命令内容, 这里要逆序
    // 默认从低位处理，这里要从高位处理
    for (auto it = commandContent.rbegin(); it != commandContent.rend(); ++it) {
//        std::cout << *it << std::endl;
        write_uint32(&command[currentIndex],  *it);
        currentIndex += 4;
    }


    // 命令尾
    command[currentIndex++] = 0x00;
    command[currentIndex++] = 0x00;
    command[currentIndex++] = 0x55;
    command[currentIndex++] = 0xAA;

//    showCommand();
}


// 在 FpgaCommand.cpp 中实现方法
void FpgaCommand::showCommand() const {
    for (const auto& byte : command) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}
