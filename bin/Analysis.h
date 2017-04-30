#pragma once
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"


//computes correlation of two images
double correlation(cv::Mat& first_image, cv::Mat& second_image);


//corelation of sequences
std::vector<double> registration_correlation(std::vector<MRISequence>& sequences, std::ostream& output);