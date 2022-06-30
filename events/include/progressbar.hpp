#pragma once

#include <iostream>

void progressBar(int start, int end, int current) {
    int total = end - start;
    int current_ = current - start;
    // std::cout << total << ", " << current_ << std::endl;
    double ratio = ((double)current_ / (double)total)*100;
    std::cout.precision(3);
    std::cout << "                                       \r";
    std::cout << "..." << ratio << "%\r" << std::flush;
}