#ifndef FPGA_COMMAND_H
#define FPGA_COMMAND_H

#include <cstdint>
#include <vector>
#include <array>
//输出需要
#include <iostream>
#include <iomanip>

#include <winsock2.h>

//htonl（host to network long）：将 32 位主机字节顺序的整数转换为网络字节顺序的整数。
//htons（host to network short）：将 16 位主机字节顺序的短整数转换为网络字节顺序的短整数。
//ntohl（network to host long）：将网络字节顺序的 32 位整数转换为主机字节顺序的整数。
//ntohs（network to host short）：将网络字节顺序的 16 位短整数转换为主机字节顺序的短整数。


class FpgaCommand {
public:
    FpgaCommand(uint32_t commandNumber, uint32_t registerAddress, const std::vector<uint32_t>& commandContent);

    const std::vector<uint8_t>& getCommand() const;

    void fillCommand(uint32_t commandNumber, uint32_t registerAddress, const std::vector<uint32_t>& commandContent);
    void showCommand() const;

private:
    // 核心数据
    std::vector<uint8_t> command; // 使用 std::vector 代替 std::array

    uint16_t commandLength;
    uint32_t commandNumber;
    uint32_t registerAddress;
    std::vector<uint32_t> commandContent;
};

#endif // FPGA_COMMAND_H
