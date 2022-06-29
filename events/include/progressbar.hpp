#pragma once

#include <iostream>

void progressBar(int total, int current) {
    double ratio = ((double)current / (double)total)*100;
    std::cout.precision(3);
    std::cout << "                                       \r";
    std::cout << "..." << ratio << "%\r" << std::flush;
}