/*
 *  In C++, template functions are different from normal functions.
 *  you can't put them in to a C++ source file, compile them separately, and then expect the code to link.
 *  The reason for this is that C++ templates aren't code - they're templates for code - and are only instantiated and compiled when they're needed.
 *  To fix this, move the implementations into the header file instead of having them in a separate cpp file.
 */
//#pragma once

#ifndef CUSTOMHARRIS_H
#define CUSTOMHARRIS_H

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono> // For timing the execution time
#include <fstream>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

template <class T>
class HarrisCornerDetector {
public:
    HarrisCornerDetector(int kSize, int bSize, int height, int width, const int datatype);
    ~HarrisCornerDetector();

    //int pascal(int k, int n);
    //int factorial(int n);
    void square(cv::Mat &input, cv::Mat &output);
    void multiply(const cv::Mat &input1, const cv::Mat &input2, cv::Mat &output);
    void multFactor(cv::Mat &input, T factor);
    void subtract(const cv::Mat &input1, const cv::Mat &input2, cv::Mat &output);
    void add(const cv::Mat &input1, const cv::Mat &input2, cv::Mat &output);
    void Conv2D(const cv::Mat &input, cv::Mat &output, const cv::Mat &kernel, std::string mode);
    void separableConv2D(const cv::Mat &input, cv::Mat &output, const cv::Mat &kernel_h, const cv::Mat &kernel_v, std::string mode);
    void generateHarrisScore(const cv::Mat &image);
    void generateWithShiTomasi(const cv::Mat &image);
    //void generateSobel();
    //void generateGauss(double sigma);

    int kernelSize;   
    int blockSize; // Same as OpenCV's cornerHarris' parameter blockSize
    //int l2;
    //double sigma;
    double gaussCoeff = 1/273;
    double harrisFactor = 0.04;
    int dataType;

    //cv::Mat Dx; // 1-D Difference Window 
    //cv::Mat Sx; // 1-D Smoothing Window
    cv::Mat harrisScore; // 2D
    cv::Mat harrisScoreNormalised; // 2D
    cv::Mat sobel_x_3; // 2D
    cv::Mat sobel_y_3; // 2D
    cv::Mat sobel_x_5; // 2D
    cv::Mat sobel_y_5; // 2D
    cv::Mat sobel_x_h_3; // 1D
    cv::Mat sobel_x_v_3; // 1D
    cv::Mat sobel_y_h_3; // 1D
    cv::Mat sobel_y_v_3; // 1D
    cv::Mat sobel_x_h_5; // 1D
    cv::Mat sobel_x_v_5; // 1D
    cv::Mat sobel_y_h_5; // 1D
    cv::Mat sobel_y_v_5; // 1D
    cv::Mat Ix; // 2D
    cv::Mat Iy; // 2D
    cv::Mat Ixx; // 2D
    cv::Mat Iyy; // 2D
    cv::Mat Ixy; // 2D
    cv::Mat gIxx; // 2D
    cv::Mat gIyy; // 2D
    cv::Mat gIxy; // 2D
    cv::Mat Gauss_3; // 2D
    cv::Mat Gauss_5; // 2D
    //cv::Mat Gauss_h; // 1D
    //cv::Mat Gauss_v; // 1D
    //std::vector<std::vector<double>> Gauss = {{1,  4,  7,  4,  1},
    //                                          {4,  16, 26, 16, 4},
    //                                          {7,  26, 41, 26, 7},
    //                                          {4,  16, 26, 16, 4},
    //                                          {1,  4,  7,  4,  1}};
};

void print(cv::Mat &matrix);

#endif

template <typename T>
HarrisCornerDetector<T>::HarrisCornerDetector(int kSize, int wSize, int height, int width, const int datatype) {
    if (kSize == 3 || kSize == 5) kernelSize = kSize;
    else throw std::invalid_argument("kSize can only be either 3 or 5");
    blockSize = wSize;
    dataType = datatype;

    //Sx = cv::Mat::zeros(1, kernelSize, dataType);
    //Dx = cv::Mat::zeros(1, kernelSize, dataType);
    sobel_x_3 = (cv::Mat_<T>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    sobel_y_3 = (cv::Mat_<T>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
    sobel_x_5 = (cv::Mat_<T>(5, 5) << 1, 2, 0, -2, -1, 4, 8, 0, -8, -4, 6, 12, 0, -12, -6, 4, 8, 0, -8, -4, 1, 2, 0, -2, -1);
    sobel_y_5 = (cv::Mat_<T>(5, 5) << 1, 4, 6, 4, 1, 2, 8, 12, 8, 2, 0, 0, 0, 0, 0, -2, -8, -12, -8, -2, -1, -4, -6, -4, -1);
    sobel_x_h_3 = (cv::Mat_<T>(1, 3) << -1, 0, 1);
    sobel_x_v_3 = (cv::Mat_<T>(1, 3) << 1, 2, 1);
    sobel_y_h_3 = (cv::Mat_<T>(1, 3) << 1, 2, 1);
    sobel_y_v_3 = (cv::Mat_<T>(1, 3) << 1, 0, -1);
    sobel_x_h_5 = (cv::Mat_<T>(1, 5) << 1, 2, 0, -2, -1);
    sobel_x_v_5 = (cv::Mat_<T>(1, 5) << 1, 4, 6, 4, 1);
    sobel_y_h_5 = (cv::Mat_<T>(1, 5) << 1, 4, 6, 4, 1);
    sobel_y_v_5 = (cv::Mat_<T>(1, 5) << 1, 2, 0, -2, -1);
    //harrisScore = cv::Mat::zeros(height, width, dataType);
    harrisScoreNormalised = cv::Mat::zeros(height, width, CV_8UC1);
    Ix = cv::Mat::zeros(height, width, dataType);
    Iy = cv::Mat::zeros(height, width, dataType);
    Ixx = cv::Mat::zeros(height, width, dataType);
    Iyy = cv::Mat::zeros(height, width, dataType);
    Ixy = cv::Mat::zeros(height, width, dataType);
    gIxx = cv::Mat::zeros(height, width, dataType);
    gIyy = cv::Mat::zeros(height, width, dataType);
    gIxy = cv::Mat::zeros(height, width, dataType);
    Gauss_3 = (cv::Mat_<T>(3, 3) << 1, 2, 1, 2, 4, 2, 1, 2, 1);
    multFactor(Gauss_3, (float)1/16);
    Gauss_5 = (cv::Mat_<T>(5, 5) << 1, 4, 6, 4, 1, 4, 16, 24, 16, 4, 6, 24, 36, 24, 6, 4, 16, 24, 16, 4, 1, 4, 6, 4, 1);
    multFactor(Gauss_5, (float)1/256);
    //Gauss_h = cv::Mat::zeros(1, kernelSize, dataType);
    //Gauss_v = cv::Mat::zeros(1, kernelSize, dataType);

    //for (int i = 0; i < kernelSize; i++) {
    //    Sx.at<T>(1, i) = factorial(kernelSize - 1)/(factorial(kernelSize - 1 - i) * factorial(i));
    //    Dx.at<T>(1, i) = pascal(i, kernelSize - 2) - pascal(i-1, kernelSize - 2);
    //}
    
    //generateSobel(); // Create the Sobel Operator;
    //generateGauss(1);
}

template <typename T>
HarrisCornerDetector<T>::~HarrisCornerDetector() {
    //Sx.release();
    //Dx.release();
    sobel_x_3.release();
    sobel_y_3.release();
    sobel_x_5.release();
    sobel_y_5.release();
    sobel_x_h_3.release();
    sobel_x_v_3.release();
    sobel_y_h_3.release();
    sobel_y_v_3.release();
    sobel_x_h_5.release();
    sobel_x_v_5.release();
    sobel_y_h_5.release();
    sobel_y_v_5.release();
    harrisScore.release();
    harrisScoreNormalised.release();
    Ix.release();
    Iy.release();
    Ixx.release();
    Iyy.release();
    Ixy.release();
    gIxx.release();
    gIyy.release();
    gIxy.release();
    //Gauss.release();
    //Gauss_h.release();
    //Gauss_v.release();
}

//template <typename T>
//int HarrisCornerDetector<T>::pascal(int k, int n) {
//    if (k >= 0 && k <= n) return factorial(n)/(factorial(n-k)*factorial(k));
//    else return 0;
//}

//template <typename T>
//int HarrisCornerDetector<T>::factorial(int n) {
//    if (n > 1) return n * factorial(n-1);
//    else return 1;
//}

template <typename T>
void HarrisCornerDetector<T>::square(cv::Mat &input, cv::Mat &output) {
    if (input.rows != 0 && input.cols != 0) {
        for (int i = 0; i < input.rows; i++) {
            for (int j = 0; j < input.cols; j++) {
                output.at<T>(i, j) = input.at<T>(i, j) * input.at<T>(i, j);
            }
        }
    }
}

template <typename T>
void HarrisCornerDetector<T>::multiply(const cv::Mat &input1, const cv::Mat &input2, cv::Mat &output) {
    // Check if input and output dimension matches
    if (input1.rows == input2.rows && input1.rows == output.rows &&
        input1.cols == input2.cols && input1.cols == output.cols) {

        // elementwise multiplication
        for (int i = 0; i < input1.rows; i++) {
            for (int j = 0; j < input1.cols; j++) {
                output.at<T>(i, j) = input1.at<T>(i,j) * input2.at<T>(i, j);
            }
        }
    }
}

template <typename T>
void HarrisCornerDetector<T>::multFactor(cv::Mat &input, T factor) {
    if (input.rows != 0 && input.cols != 0) {
        for (int i = 0; i < input.rows; i++) {
            for (int j = 0; j < input.cols; j++) {
                input.at<T>(i, j) *= factor;
            }
        }
    }
}

template <typename T>
void HarrisCornerDetector<T>::subtract(const cv::Mat &input1, const cv::Mat &input2, cv::Mat &output) {
    if (input1.rows == input2.rows && input1.rows == output.rows &&
        input1.cols == input2.cols && input1.cols == output.cols) {

        for (int i = 0; i < input1.rows; i++) {
            for (int j = 0; j < input1.cols; j++) {
                output.at<T>(i, j) = input1.at<T>(i,j) - input2.at<T>(i, j);
            }
        }
    }
}

template <typename T>
void HarrisCornerDetector<T>::add(const cv::Mat &input1, const cv::Mat &input2, cv::Mat &output) {
    if (input1.rows == input2.rows && input1.rows == output.rows &&
        input1.cols == input2.cols && input1.cols == output.cols) {

        for (int i = 0; i < input1.rows; i++) {
            for (int j = 0; j < input1.cols; j++) {
                output.at<T>(i, j) = input1.at<T>(i,j) + input2.at<T>(i, j);
            }
        }
    }
}

template <typename T>
void HarrisCornerDetector<T>::Conv2D(const cv::Mat &input, cv::Mat &output, const cv::Mat &kernel, std::string mode) {
    int inputRow = input.rows;
    int inputCol = input.cols;
    int kernelRow = kernel.rows;
    int kernelCol = kernel.cols;
    int kernelCenterX = kernelCol/2;
    int kernelCenterY = kernelRow/2;
    auto sum = 0;

    cv::Mat flipped;
    cv::flip(kernel, flipped, -1);

    for (int i = 0; i < inputRow; i++) {
        for (int j = 0; j < inputCol; j++) {
            sum = 0;
            for (int kRow = 0; kRow < kernelRow; kRow++) {
                for (int kCol = 0; kCol < kernelCol; kCol++) {
                    int x = j - kernelCenterX + kCol;
                    int y = i - kernelCenterY + kRow;
                    if (x >= 0 && x < inputCol && y >= 0 && y < inputRow)
                        sum += input.at<T>(y, x) * flipped.at<T>(kRow, kCol);
                }
            }
            output.at<T>(i, j) = sum;
        }
    }
}

template <typename T>
void HarrisCornerDetector<T>::separableConv2D(const cv::Mat &input, cv::Mat &output, const cv::Mat &kernel_h, const cv::Mat &kernel_v, std::string mode) {
    int inputRow = input.rows;
    int inputCol = input.cols;
    int kernelRow = kernel_v.cols; // Mat format is 1*kernel_size
    int kernelCol = kernel_v.cols; // Mat format is 1*kernel_size
    int kernelCenterX = kernelCol/2;
    int kernelCenterY = kernelRow/2;
    auto sum = 0;

    cv::Mat intermediate(input.rows, input.cols, dataType, cv::Scalar(0));

    // Vertical
    for (int i = 0; i < inputRow; i++) {
        for (int j = 0; j < inputCol; j++) {
            sum = 0;
            for (int kRow = 0; kRow < kernelRow; kRow++) {
                int y = i - kernelCenterY + kRow;
                if (y >= 0 && y < inputRow)
                    sum += input.at<T>(y, j) * kernel_v.at<T>(1, kRow);
            }
            intermediate.at<T>(i, j) = sum;
        }
    }

    // Horizontal
    for (int i = 0; i < inputRow; i++) {
        for (int j = 0; j < inputCol; j++) {
            sum = 0;
            for (int kCol = 0; kCol < kernelCol; kCol++) {
                int x = j - kernelCenterX + kCol;
                if (x >= 0 && x < inputCol)
                    sum += intermediate.at<T> (i, x) * kernel_h.at<T>(1, kCol);
            }
            output.at<T>(i, j) = sum;
        }
    }
}

// template <typename T>
// void HarrisCornerDetector<T>::generateHarrisScore(const cv::Mat &image) {
//     if (kernelSize == 3) {
//         Conv2D(image, Ix, sobel_x_3, "");
//         Conv2D(image, Iy, sobel_y_3, "");
//     } else {
//         Conv2D(image, Ix, sobel_x_h_5, "");
//         Conv2D(image, Iy, sobel_y_h_5, "");
//     }

//     multiply(Ix, Iy, Ixy);  // Ixy = Ix * Iy
//     square(Ix, Ixx);        // Ixx = Ix^2
//     square(Iy, Iyy);        // Iyy = Iy^2

//     if (kernelSize == 3) {
//         Conv2D(Ixx, gIxx, Gauss_3, "");
//         Conv2D(Iyy, gIyy, Gauss_3, "");
//         Conv2D(Ixy, gIxy, Gauss_3, "");
//     } else {
//         Conv2D(Ixx, gIxx, Gauss_5, "");
//         Conv2D(Iyy, gIyy, Gauss_5, "");
//         Conv2D(Ixy, gIxy, Gauss_5, "");
//     }

//     // score = det(M) - k*trace(M)^2
//     // Can calculate as the entire matrix or use window sum
//     cv::Mat det (Ix.rows, Ix.cols, dataType, cv::Scalar(0));
//     cv::Mat temp1(Ix.rows, Ix.cols, dataType, cv::Scalar(0));
//     cv::Mat temp2(Ix.rows, Ix.cols, dataType, cv::Scalar(0));
//     multiply(gIxx, gIyy, temp1);
//     square(gIxy, temp2);
//     subtract(temp1, temp2, det);

//     cv::Mat trace (Ix.rows, Ix.cols, dataType, cv::Scalar(0));
//     add(gIxx, gIyy, temp1);
//     square(temp1, trace);
//     multFactor(trace, 0.04);

//     subtract(det, trace, harrisScore);
// }

template <typename T>
void HarrisCornerDetector<T>::generateHarrisScore(const cv::Mat &image) {
    if (kernelSize == 3) {
        cv::filter2D(image, Ix, -1, sobel_x_3, cv::Point(-1, -1));
        cv::filter2D(image, Iy, -1, sobel_y_3, cv::Point(-1, -1));
    } else {
        cv::filter2D(image, Ix, -1, sobel_x_5, cv::Point(-1, -1));
        cv::filter2D(image, Iy, -1, sobel_y_5, cv::Point(-1, -1));
    }

    Ixx = Ix.mul(Ix);
    Iyy = Iy.mul(Iy);
    Ixy = Ix.mul(Iy);

    if (kernelSize == 3) {
        cv::filter2D(Ixx, gIxx, -1, Gauss_3, cv::Point(-1, -1));
        cv::filter2D(Iyy, gIyy, -1, Gauss_3, cv::Point(-1, -1));
        cv::filter2D(Ixy, gIxy, -1, Gauss_3, cv::Point(-1, -1));
    } else {
        cv::filter2D(Ixx, gIxx, -1, Gauss_5, cv::Point(-1, -1));
        cv::filter2D(Iyy, gIyy, -1, Gauss_5, cv::Point(-1, -1));
        cv::filter2D(Ixy, gIxy, -1, Gauss_5, cv::Point(-1, -1));
    }
    cv::Mat temp1 = gIxx.mul(gIyy);
    cv::Mat temp2 = gIxy.mul(gIxy);
    cv::Mat det = temp1 - temp2;

    temp1 = gIxx + gIyy;
    cv::Mat trace = temp1.mul(temp1);
    trace = trace * harrisFactor;

    harrisScore = det - trace;
}

template <typename T>
void HarrisCornerDetector<T>::generateWithShiTomasi(const cv::Mat &image) {
    if (kernelSize == 3) {
        Conv2D(image, Ix, sobel_x_3, "");
        Conv2D(image, Iy, sobel_y_3, "");
    } else {
        Conv2D(image, Ix, sobel_x_h_5, "");
        Conv2D(image, Iy, sobel_y_h_5, "");
    }

    //multiply(Ix, Iy, Ixy);  // Ixy = Ix * Iy
    square(Ix, Ixx);             // Ixx = Ix^2
    square(Iy, Iyy);             // Iyy = Iy^2

    for (int i=0; i < image.rows; i++) {
        for (int j=0; j < image.cols; j++) {
            int ymin = std::max(0, i - blockSize/2);
            int ymax = std::min(image.rows, i + blockSize/2);
            int xmin = std::max(0, j - blockSize/2);
            int xmax = std::min(image.cols, j + blockSize/2);

            T Sxx = 0;
            T Syy = 0;
            //T Sxy = 0;

            for (int row=ymin; row < ymax; row++) {
                for (int col=xmin; col < xmax; col++) {
                    Sxx += Ixx.at<T>(row, col);
                    Syy += Iyy.at<T>(row, col);
                    //Sxy += Ixy.at<T>(row, col);
                }
            }

            //T det = (Sxx * Syy) - (Sxx * Sxx);
            //T trace = Sxx + Syy;
            //harrisScore.at<T>(i, j) = (det - harrisFactor*(trace * trace));
            harrisScore.at<T>(i, j) = std::min(Sxx, Syy);
        }
    }
}

/*template <typename T>
void HarrisCornerDetector<T>::generateSobel() {
    // Generate Sobel_X (Vertical)
    for (int win_x = 0; win_x < kernelSize; win_x++) {
        for (int win_y = 0; win_y < kernelSize; win_y++) {
            sobel_x.at<T>(win_x, win_y) = Sx.at<T>(1, win_x) * Dx.at<T>(1, win_y);
            if (win_y == 0) sobel_x_v.at<T>(1, win_x) = Sx.at<T>(1, win_x) * Dx.at<T>(1, win_y);
            if (win_x == 0) sobel_x_h.at<T>(1, win_y) = Sx.at<T>(1, win_x) * Dx.at<T>(1, win_y);
        }
    }

    // Generate Sobel_Y (Horizontal)
    for (int win_x = 0; win_x < kernelSize; win_x++) {
        for (int win_y = 0; win_y < kernelSize; win_y++) {
            sobel_y.at<T>(win_x, win_y) = Sx.at<T>(1, win_y) * Dx.at<T>(1, win_x);
            if (win_y == 0) sobel_y_h.at<T>(1, win_x) = Sx.at<T>(1, win_x) * Dx.at<T>(1, win_y);
            if (win_x == 0) sobel_y_v.at<T>(1, win_y) = Sx.at<T>(1, win_x) * Dx.at<T>(1, win_y);
        }
    }
}

template <typename T>
void HarrisCornerDetector<T>::generateGauss(double sigma) {
    double coeff = 2.0*M_PI*sigma*sigma;
    double exp_coeff = 2.0*sigma*sigma;
    double sum = 0.0; // This is used for normalisation
    double temp = 0.0;
    int kSize = kernelSize/2;

    for (int x = -kSize; x <= kSize; x++) { 
        //std::cout << "temp" << std::endl;
        for (int y = -kSize; y <= kSize; y++) {
            temp = x*x + y*y;
            //std::cout << temp << " ";
            Gauss.at<T>(y+kSize, x+kSize) = exp(-temp/exp_coeff)/coeff;
            //std::cout << Gauss.at<T>(y+kSize, x+kSize) << " ";
            sum += Gauss.at<T>(y+kSize, x+kSize);

            if (y == 0) Gauss_v.at<T>(1, x) = Gauss.at<T>(y+kSize, x+kSize);
            if (x == 0) Gauss_h.at<T>(1, y) = Gauss.at<T>(y+kSize, x+kSize);
        }
        //std::cout << std::endl;
    }
    //std::cout << sum << std::endl;
    //for (auto &row: Gauss) {
    //    for (auto &col: row) {
    //        col /= sum;
    //    }
    //}
    //std::cout << "Gauss" << std::endl;
    //print<double>(Gauss);
}*/