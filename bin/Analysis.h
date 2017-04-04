#pragma once
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"

double correlation(cv::Mat& first_image, cv::Mat& second_image);

double registration_correlation(std::vector<std::shared_ptr<MRISequence>> sequences);