// g++ -std=c++11 -w -O2 -I /usr/local/Cellar/opencv/4.5.5_1/include/opencv4/ -o customharris customharris.cpp luvharris.cpp -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_videoio
#include "customharris.hpp"
#include "luvharris.hpp"
#include <fstream>
#include <chrono>

void print(cv::Mat &matrix) {
    std::cout.precision(10);

    std::cout << matrix << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
/*
int main() {
    cv::Mat input_img = cv::imread("1kjl.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat input_img_rgb = cv::imread("1kjl.jpg", cv::IMREAD_COLOR);
    cv::Mat output_img_rgb = input_img_rgb.clone();

    std::cout << (int)input_img.at<unsigned char>(0,0) << std::endl;

    HarrisCornerDetector<float> dummy (3, 2, input_img.rows, input_img.cols, CV_32FC1);
    

    cv::Mat output, output_normalised;

    cv::cornerHarris(input_img, output, 2, 3, 0.04);
    cv::normalize(output, output_normalised, 0, 255, cv::NORM_MINMAX, CV_8UC1, cv::Mat());
    
    for (int i = 0; i < output.rows; i++) {
        for (int j = 0; j < output.cols; j++) {
            if (output.at<float> (i, j) > 0.01) {
                cv::circle(input_img_rgb, cv::Point(j, i), 3, cv::Scalar(0,0,255));
                //input_img_rgb.at<cv::Vec3b>(i, j) = cv::Vec3b(0,0,255);
            }
        }
    }
    cv::imwrite("baseline_cornerHarris_output.jpg", input_img_rgb);

    dummy.generateHarrisScore(input_img);
    //cv::normalize(dummy.harrisScore, output_normalised, 0, 255, cv::NORM_MINMAX, CV_8UC1, cv::Mat());
    
    std::cout << input_img.rows << ", " << input_img.cols << std::endl;
    std::cout << dummy.harrisScore.rows << ", " << dummy.harrisScore.cols << std::endl;
    double min, max;
    cv::minMaxIdx(dummy.harrisScore, &min, &max);
    for (int i = 0; i < dummy.harrisScore.rows; i++) {
        for (int j = 0; j < dummy.harrisScore.cols; j++) {
            //std::cout << dummy.harrisScore.at<> (i, j) << "|";
            if (dummy.harrisScore.at<unsigned char> (i, j) > 240) {
                cv::circle(output_img_rgb, cv::Point(j, i), 3, cv::Scalar(0,0,255));
                //output_img_rgb.at<cv::Vec3b>(i, j) = cv::Vec3b(0,0,255);
            }
        }
    }
    std:: cout << max << std::endl;
    cv::imwrite("custom_cornerHarris_output.jpg", output_img_rgb);
    //cv::imwrite("custom_cornerHarris_output.jpg", dummy.harrisScore);

    int count = 0;
    int total = 100;
    while (count < total) {
        progressBar(total, count);

        for (int i = 0; i < dummy.harrisScore.rows; i++) {
            for (int j = 0; j < dummy.harrisScore.cols; j++) {
                //std::cout << dummy.harrisScore.at<> (i, j) << "|";
                if (dummy.harrisScore.at<unsigned char> (i, j) > 240) {
                    cv::circle(output_img_rgb, cv::Point(j, i), 3, cv::Scalar(0,0,255));
                    //output_img_rgb.at<cv::Vec3b>(i, j) = cv::Vec3b(0,0,255);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        count++;
    }
    std::cout << std::endl;

    std::ofstream outfile;
    outfile.open("CPP_Input.csv", std::ios_base::app);
    if (!outfile) {
        std::cout << "Failed to open All_events.txt" << std::endl;
        return -1;
    }
    outfile<< cv::format(input_img, cv::Formatter::FMT_CSV) << std::endl;
    outfile.close();

    outfile.open("CPP_Harris_Response.csv", std::ios_base::app);
    if (!outfile) {
        std::cout << "Failed to open All_events.txt" << std::endl;
        return -1;
    }
    outfile<< cv::format(dummy.harrisScore, cv::Formatter::FMT_CSV) << std::endl;
    outfile.close();

    outfile.open("CPP_Ix.csv", std::ios_base::app);
    if (!outfile) {
        std::cout << "Failed to open All_events.txt" << std::endl;
        return -1;
    }
    outfile<< cv::format(dummy.Ix, cv::Formatter::FMT_CSV) << std::endl;
    outfile.close();

    // outfile.open("CPP_Ixx.csv", std::ios_base::app);
    // if (!outfile) {
    //     std::cout << "Failed to open All_events.txt" << std::endl;
    //     return -1;
    // }
    // for (int i = 0; i < dummy.Ixx.rows; i++) {
    //     for (int j = 0; j < dummy.Ixx.cols; j++) {
    //         outfile <<  dummy.Ixx.at<float> (i, j) << ",";
    //     }
    //     outfile << "\n";
    // }
    // outfile.close();

    // outfile.open("CPP_gIxx.csv", std::ios_base::app);
    // if (!outfile) {
    //     std::cout << "Failed to open All_events.txt" << std::endl;
    //     return -1;
    // }
    // for (int i = 0; i < dummy.gIxx.rows; i++) {
    //     for (int j = 0; j < dummy.gIxx.cols; j++) {
    //         outfile <<  dummy.gIxx.at<float> (i, j) << ",";
    //     }
    //     outfile << "\n";
    // }
    // outfile.close();

    // return 0;
}
*/