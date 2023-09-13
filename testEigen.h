#ifndef TESTEIGEN_H
#define TESTEIGEN_H

#include <iostream>
#include <chrono>
#include <thread>
#include <Eigen/Dense>

// 在这里定义其他函数，除了 main 函数
void testEigen() {

    constexpr uint32_t sampleRate = 1; // 平均值采样率（每秒）

    constexpr uint32_t numRows = 5; // 矩阵的行数
    constexpr uint32_t numCols = 10; // 矩阵的列数

    const int t = 1000 / sampleRate;

    Eigen::MatrixXd data(numRows, numCols);
    data.setRandom(); // 使用随机数填充矩阵

    while (true) {
        Eigen::VectorXd a48 = data.colwise().mean(); // 计算每行的平均值

        // 打印a48的shape
        std::cout << "a48的shape：" << a48.rows() << "x" << a48.cols() << std::endl;
        std::cout << "均值后：" << std::endl;
            for (int i = 0; i < numCols; ++i) {
            std::cout << a48(i) << " ";
        }
        std::cout << std::endl;

        // 对整个向量 a48 减去 5 并乘以 2
        a48 = (a48.array() - 5) * 2;

        // 打印a48的转化值
        std::cout << "a48的shape：" << a48.rows() << "x" << a48.cols() << std::endl;
            std::cout << "转化后：" << std::endl;
            for (int i = 0; i < numCols; ++i) {
            std::cout << a48(i) << " ";
        }
        std::cout << std::endl;

        // 重新设置data的随机数
        data.setRandom();
        // 等待a毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(t));
    }
}

#endif // TESTEIGEN_H
