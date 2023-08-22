#ifndef COMMANDCONTENT_H
#define COMMANDCONTENT_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <bitset>
#include <algorithm>

class CommandContent {
private:
    std::vector<uint32_t> data;
public:
    // 命令内容搞糟函数
    CommandContent(const CommandContent& other);
    CommandContent(const std::vector<uint32_t>& newData);
    CommandContent(const unsigned char* bufferPtr, size_t n);
    // 根据提供的新数据填充data向量
    void fillContent(const std::vector<uint32_t>& newData);
    void fillFromBuffer(const unsigned char* bufferPtr, size_t n);
    // 重置data向量中的所有元素为指定值
    void reset(uint32_t value);

    // 命令内容设置 解析
    // 1bit读写
    void setBitValue(int index, bool value);
    bool getBitValue(int index) const;

    // 连续bit读写， start+length, 直接整体给值
    void setBitsRange(int startIndex, int m, uint32_t value);
    void setBitsRangeFromTo(int fromIndex, int toIndex, uint32_t value);

    // 连续bit读写， start+end，直接整体给值
    uint32_t getBitsRange(int startIndex, int m) const;
    uint32_t getBitsRangeFromTo(int fromIndex, int toIndex) const;

    // 十六进制显示 data
    void hexShow() const;
    // 二进制显示 data
    void binaryShow() const;

    const std::vector<uint32_t>& getData() const;

};

#endif  // COMMANDCONTENT_H
