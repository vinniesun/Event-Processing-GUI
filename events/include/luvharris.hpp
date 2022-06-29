//#pragma once

#ifndef LUVHARRIS_H
#define LUVHARRIS_H

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
//#include <opencv2/core/mat.hpp>

template <typename T>
void convertVectorToMat(std::vector<std::vector<T>> &TOS, cv::Mat &lookup);

void generateLookup(cv::Mat &input_TOS, cv::Mat &output, int blockSize, int apertureSize, double k);
bool luvHarris(cv::Mat &harrisLookup, int x, int y, int t, bool p, int threshold);

#endif

/*
 * This function converts a 2D vector to OpenCV's cv::Mat.
 */
template <typename T>
void convertVectorToMat(std::vector<std::vector<T>> &TOS, cv::Mat &lookup) {
    //cv::Mat lookup(TOS.size(), TOS[0].size(), CV_8UC1); // CV_8UC1 means 8-bits, Unsigned integer, 1 Channel
    //std::cout << lookup.rows << std::endl;
    //std::cout << lookup.cols << std::endl;
    for (int row = 0; row < lookup.rows; row++) {
        for (int col = 0; col < lookup.cols; col++) {
            //std::cout << row << "," << col << " ";
            //std::cout << TOS[row][col] << "\t";
            lookup.at<int>(row, col) = TOS[row][col];
            //std::cout << lookup.at<int>(row, col) << "\t";
        }
        //std::cout << std::endl;
    }
    //return lookup;
}