#ifndef COMMANDCONTENT_H
#define COMMANDCONTENT_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <bitset>

class CommandContent {
private:
    std::vector<uint32_t> data;
public:
    // 命令内容
    CommandContent(const CommandContent& other);
    CommandContent(const std::vector<uint32_t>& newData);
    // 命令内容设置 解析
    void setBitValue(int index, bool value);
    void setBitsRange(int startIndex, int m, uint32_t value);
    void setBitsRangeFromTo(int fromIndex, int toIndex, uint32_t value);
    bool getBitValue(int index) const;
    uint32_t getBitsRange(int startIndex, int m) const;
    uint32_t getBitsRangeFromTo(int fromIndex, int toIndex) const;
    void hexShow() const;
    void binaryShow() const;
    const std::vector<uint32_t>& getData() const;
    // 指令构造
};

#endif  // COMMANDCONTENT_H
