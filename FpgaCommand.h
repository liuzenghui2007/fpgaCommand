#ifndef FPGA_COMMAND_H
#define FPGA_COMMAND_H

#include <cstdint>
#include <vector>
#include <array>
//输出需要
#include <iostream>
#include <iomanip>

class FpgaCommand {
public:
    FpgaCommand(uint32_t commandNumber, uint32_t registerAddress, const std::vector<uint32_t>& commandContent);

    const std::vector<uint8_t>& getCommandData() const;
    void showCommand() const;

private:
    uint16_t commandLength;
    uint32_t commandNumber;
    uint32_t registerAddress;
    std::vector<uint32_t> commandContent;
    std::vector<uint8_t> commandData; // 使用 std::vector 代替 std::array

    void fillCommandData();
    void fillUint32ToBytes(uint32_t value, std::vector<uint8_t>& bytes, int index);

};

#endif // FPGA_COMMAND_H
