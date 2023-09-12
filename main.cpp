#include <iostream>
#include <chrono>
#include <thread>
#include <NumCpp.hpp>

using namespace std;
using namespace nc;

int main() {
    constexpr uint32_t a = 1000;    // 生成矩阵的时间间隔（毫秒）
    constexpr uint32_t b = 5;      // 每次生成的矩阵数量
    constexpr uint32_t c = 5;      // 每c个矩阵做一次平均
    constexpr uint32_t sampleRate = 1; // 平均值采样率（每秒）

    constexpr uint32_t numRows = 50; // 矩阵的行数
    constexpr uint32_t numCols = 10; // 矩阵的列数

    NdArray<int> data = random::randInt<int>({numRows, numCols}, 0, 100);
    int counter = 0;

    while (true) {


        auto a48 = mean(data, nc::Axis::ROW); // 计算每行的平均值

        // 打印a48的shape
        cout << "a48的shape：" << a48.shape() << endl;
        cout << "均值后：" << endl;
        for (int i = 0; i < numCols; ++i) {
            cout << a48[i] << " ";
        }
        cout << endl;
        // 重新设置data的随机数
        data = random::randInt<int>({numRows, numCols}, 0, 100);
        // 等待a毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(a));
    }

    return 0;
}
