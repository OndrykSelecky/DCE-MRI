#pragma once
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"

double correlation(cv::Mat& first_image, cv::Mat& second_image);

std::vector<double> registration_correlation(std::vector<MRISequence>& sequences, std::ofstream& output);