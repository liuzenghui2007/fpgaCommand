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
    CommandContent(const CommandContent& other);
    CommandContent(const std::vector<uint32_t>& newData);

    void setBitValue(int index, bool value);
    void setBitsRange(int startIndex, int m, uint32_t value);
    void setBitsRangeFromTo(int fromIndex, int toIndex, uint32_t value);
    bool getBitValue(int index) const;
    uint32_t getBitsRange(int startIndex, int m) const;
    uint32_t getBitsRangeFromTo(int fromIndex, int toIndex) const;
    void hexShow() const;
    void binaryShow() const;
};

#endif  // COMMANDCONTENT_H
