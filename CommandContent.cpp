#include "CommandContent.h"


CommandContent::CommandContent(const CommandContent& other) : data(other.data) {}

CommandContent::CommandContent(const std::vector<uint32_t>& newData) : data(newData) {}

CommandContent::CommandContent(const unsigned char* bufferPtr, size_t n) {
    if (n % 4 != 0) {
        throw std::invalid_argument("Length must be a multiple of 4");
    }

    data.resize(n / 4); // Assuming 4 bytes represent one uint32_t

    std::transform(bufferPtr, bufferPtr + n, data.begin(), [](unsigned char byte) {
        return static_cast<uint32_t>(byte);
    });

    // Rearrange bytes for endianness if necessary
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = (data[i] & 0xFF) << 24 |
                  (data[i] & 0xFF00) << 8 |
                  (data[i] & 0xFF0000) >> 8 |
                  (data[i] & 0xFF000000) >> 24;
    }
}

void CommandContent::fillContent(const std::vector<uint32_t>& newData) {
    data = newData;
}

void CommandContent::fillFromBuffer(const unsigned char* bufferPtr, size_t n) {
    if (n % 4 != 0) {
        throw std::invalid_argument("Length must be a multiple of 4");
    }

    data.resize(n / 4); // Assuming 4 bytes represent one uint32_t

    for (size_t i = 0; i < n; i += 4) {
        uint32_t value = (static_cast<uint32_t>(bufferPtr[i]) << 24) |
                         (static_cast<uint32_t>(bufferPtr[i + 1]) << 16) |
                         (static_cast<uint32_t>(bufferPtr[i + 2]) << 8) |
                         static_cast<uint32_t>(bufferPtr[i + 3]);
        data[i / 4] = value;
    }


}


void CommandContent::setBitValue(int index, bool value) {
    int arrayIndex = index / 32;
    int bitOffset = index % 32;

    if (arrayIndex >= data.size()) {
        throw std::out_of_range("Index out of range");
    }

    if (value) {
        data[arrayIndex] |= (1 << bitOffset);
    } else {
        data[arrayIndex] &= ~(1 << bitOffset);
    }
}

void CommandContent::setBitsRange(int startIndex, int m, uint32_t value) {
    for (int i = 0; i < m; ++i) {
        setBitValue(startIndex + i, value & 1);
        value >>= 1;
    }
}

void CommandContent::setBitsRangeFromTo(int fromIndex, int toIndex, uint32_t value) {
    if (fromIndex > toIndex) {
        std::swap(fromIndex, toIndex);
    }

    for (int i = fromIndex; i <= toIndex; ++i) {
        setBitValue(i, value & 1);
        value >>= 1;
    }
}

bool CommandContent::getBitValue(int index) const {
    int arrayIndex = index / 32;
    int bitOffset = index % 32;

    if (arrayIndex >= data.size()) {
        throw std::out_of_range("Index out of range");
    }

    return (data[arrayIndex] >> bitOffset) & 1;
}

uint32_t CommandContent::getBitsRange(int startIndex, int m) const {
    if ((startIndex < 0) || (startIndex + m > data.size() * 32)) {
        throw std::out_of_range("Index out of range");
    }

    uint32_t result = 0;
    for (int i = 0; i < m; ++i) {
        result |= (static_cast<uint32_t>(getBitValue(startIndex + i)) << i);
    }

    return result;
}

uint32_t CommandContent::getBitsRangeFromTo(int fromIndex, int toIndex) const {
    if (fromIndex > toIndex) {
        std::swap(fromIndex, toIndex);
    }

    uint32_t result = 0;
    for (int i = fromIndex; i <= toIndex; ++i) {
        result |= (static_cast<uint32_t>(getBitValue(i)) << (i - fromIndex));
    }

    return result;
}

uint32_t CommandContent::getState(StateBitsRange range) {
    if (range.low < 0 || range.high > 31 || range.low > range.high) {
        throw std::invalid_argument("Invalid range");
    }

//    std::cout << "range" << range.low << "," << range.high << std::endl;
    uint32_t result = getBitsRangeFromTo(range.low, range.high);
    return result;
}

void CommandContent::showHex() const {
    std::cout << "hex format" << '\n';
    for (const uint32_t& value : data) {
        std::cout << "0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << value << std::endl;
    }
}

void CommandContent::showBin() const {
    std::cout << "binary format" << '\n';
    for (const uint32_t& value : data) {
        std::cout << std::bitset<32>(value).to_string().insert(24, " ").insert(16, " ").insert(8, " ") << '\n';
    }
    std::cout << "-----------------------------------" << '\n';
}

// 返回成员变量 data
const std::vector<uint32_t>& CommandContent::getData() const {
    return data;
}

void CommandContent::reset(uint32_t value) {
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = value;
    }
}
