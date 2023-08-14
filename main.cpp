#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <bitset>

class CommandContent {
private:
    std::vector<uint32_t> data;

public:
    // 构造函数：从另一个CommandContent对象或者数据数组赋值
    CommandContent(const CommandContent& other) : data(other.data) {}
    CommandContent(const std::vector<uint32_t>& newData) : data(newData) {}

    // 设置指定索引的bit值
    void setBitValue(int index, bool value) {
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

    // 设置连续m个bit的值（根据整数值）
    void setBitsRange(int startIndex, int m, uint32_t value) {
        for (int i = 0; i < m; ++i) {
            setBitValue(startIndex + i, value & 1);
            value >>= 1;
        }
    }

    // 从起点索引到终点索引设置连续bit的值（根据整数值）
    void setBitsRangeFromTo(int fromIndex, int toIndex, uint32_t value) {
        if (fromIndex > toIndex) {
            std::swap(fromIndex, toIndex);
        }

        for (int i = fromIndex; i <= toIndex; ++i) {
            setBitValue(i, value & 1);
            value >>= 1;
        }
    }

    // 获取指定索引的bit值
    bool getBitValue(int index) const {
        int arrayIndex = index / 32;
        int bitOffset = index % 32;

        if (arrayIndex >= data.size()) {
            throw std::out_of_range("Index out of range");
        }

        return (data[arrayIndex] >> bitOffset) & 1;
    }

    // 获取从起点索引到终点索引的bit值，并将其转化为整数
    uint32_t getBitsRange(int startIndex, int m) const {
        if ((startIndex < 0) || (startIndex + m > data.size() * 32)) {
            throw std::out_of_range("Index out of range");
        }

        uint32_t result = 0;
        for (int i = 0; i < m; ++i) {
            result |= (static_cast<uint32_t>(getBitValue(startIndex + i)) << i);
        }

        return result;
    }

    // 获取从起点索引到终点索引的bit值，并将其转化为整数
    uint32_t getBitsRangeFromTo(int fromIndex, int toIndex) const {
        if (fromIndex > toIndex) {
            std::swap(fromIndex, toIndex);
        }

        uint32_t result = 0;
        for (int i = fromIndex; i <= toIndex; ++i) {
            result |= (static_cast<uint32_t>(getBitValue(i)) << (i - fromIndex));
        }

        return result;
    }

    // 以十六进制格式显示每个元素并换行
    void hexShow() const {
        std::cout << "hex format" << '\n';
        for (const uint32_t& value : data) {
            std::cout << "0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << value << std::endl;
        }
    }

    // 以二进制格式显示每个元素，每8位之间添加空格，每个元素换行
    void binaryShow() const {
        std::cout << "binary format" << '\n';
        for (const uint32_t& value : data) {
            std::cout << std::bitset<32>(value).to_string().insert(24, " ").insert(16, " ").insert(8, " ") << '\n';
        }
    }
};

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
    return 0;
}
