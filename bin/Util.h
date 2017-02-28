#pragma once
#include <vector>
#include "opencv2/opencv.hpp"

typedef std::vector<std::vector<cv::Point2f>> features;

struct triangle
{
	cv::Point2f a;
	cv::Point2f b;
	cv::Point2f c;

	std::vector<cv::Point2f> detected_points;
};


std::vector<std::string> get_file_names(std::string folder, const std::string& suffix = "");
std::vector<std::string> get_folder_names(std::string folder);