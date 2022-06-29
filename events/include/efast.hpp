//#pragma once

#ifndef EFAST_H
#define EFAST_H

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

bool eFast(std::vector<std::vector<int>> sae_, int x, int y, int t, bool p);
bool eFast(cv::Mat sae_, int x, int y, int t, bool p);

#endif