//#pragma once

#ifndef ARCSTAR_H
#define ARCSTAR_H

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

bool arcStar(std::vector<std::vector<int>> sae_, int x, int y, int t, bool p);
bool arcStar(cv::Mat sae_, int x, int y, int t, bool p);

#endif