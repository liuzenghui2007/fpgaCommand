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
    constexpr uint32_t numCols = 12; // 矩阵的列数

    const int t = 1000 / sampleRate;

    Eigen::MatrixXd data(numRows, numCols);
    Eigen::MatrixXd base(1, numCols);
    data.setRandom(); // 使用随机数填充矩阵
    base.setConstant(2.0); // 使用随机数填充基线矩阵（一维）
    // 在循环之前计算 base 复制矩阵
    Eigen::MatrixXd baseReplicated = base.replicate(numRows, 1).array();

    while (true) {
        // 使用replicate函数将base的行复制为与data相同的行数，并且然后使用array()函数执行逐元素减法操作。
        // 确保在修改后的矩阵上执行array()操作，以便逐元素地进行减法操作。
        // 原始值
        std::cout << "原始值：" << std::endl;
        std::cout << data << std::endl;


        data.array() -= baseReplicated.array();

        // 输出整个矩阵的所有值
        std::cout << "去基线后：" << std::endl;
        std::cout << data << std::endl;

        Eigen::MatrixXd a48 = data.colwise().mean(); // 计算每行的平均值
        std::cout << "a48的shape：" << a48.rows() << "x" << a48.cols() << std::endl;



        // 均值
        std::cout << "均值后：" << std::endl;
        std::cout << a48 << std::endl;

        // 转化，对整个向量 a48 减去 5 并乘以 2
        a48 = (a48.array() - 5) * 2;
        std::cout << "转电流：" << std::endl;
        std::cout << a48 << std::endl;
        std::cout << "----------------------------" << std::endl;
        // 重新设置data的随机数
        data.setRandom();
        // 等待a毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(t));
    }
}

#endif // TESTEIGEN_H
