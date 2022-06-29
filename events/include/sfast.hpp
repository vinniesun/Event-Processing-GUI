//#pragma once

#ifndef SFAST_H
#define SFAST_H

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

bool sFast(cv::Mat sae_, int x, int y, int t, bool p);
bool sFastOuterOnly(cv::Mat sae_, int x, int y, int t, bool p);

#endif