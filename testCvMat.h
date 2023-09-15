#ifndef TESTCVMAT_H
#define TESTCVMAT_H

#include <iostream>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp>

void testOpenCV() {
    constexpr uint32_t sampleRate = 1;
    constexpr uint32_t numRows = 5;
    constexpr uint32_t numCols = 12;
    const int t = 1000 / sampleRate;

    cv::Mat data(numRows, numCols, CV_64F);
    cv::Mat base(data.rows, data.cols, CV_64F, cv::Scalar::all(2.0));

    cv::randn(data, cv::Scalar::all(0), cv::Scalar::all(1)); // 使用随机数填充矩阵


    while (true) {
        // 原始值
        std::cout << "原始值：" << std::endl;
                std::cout << data << std::endl;

        data -= base; // 逐元素减法操作

        // 输出整个矩阵的所有值
        std::cout << "去基线后：" << std::endl;
            std::cout << data << std::endl;

        cv::Mat a48 = cv::Mat::zeros(1, numCols, CV_64F);

        // 计算每列的平均值
        for (int col = 0; col < numCols; col++) {
            for (int row = 0; row < numRows; row++) {
                a48.at<double>(0, col) += data.at<double>(row, col);
            }
            a48.at<double>(0, col) /= numRows;
        }

        std::cout << "a48的shape：" << a48.rows << "x" << a48.cols << std::endl;

                    // 均值
                    std::cout << "均值后：" << std::endl;
                         std::cout << a48 << std::endl;

        // 转化，对整个向量 a48 减去 5 并乘以 2
        a48 = (a48 - 5) * 2;
        std::cout << "转电流：" << std::endl;
                std::cout << a48 << std::endl;
        std::cout << "----------------------------" << std::endl;

        // 重新设置data的随机数
        cv::randn(data, cv::Scalar::all(0), cv::Scalar::all(1));

        // 等待t毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(t));
    }
}

#endif // TESTCVMAT_H
