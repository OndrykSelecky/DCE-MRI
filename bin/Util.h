#pragma once
#include <vector>
#include "opencv2/opencv.hpp"

typedef std::vector<std::vector<cv::Point2f>> features;

struct Triangle
{
	//id-s of points in features vector
	int a;
	int b;
	int c;

	//coordinates of these points (copied)
	std::vector<cv::Point2f> point_coordinates;
};


std::vector<std::string> get_file_names(std::string folder, const std::string& suffix = "");
std::vector<std::string> get_folder_names(std::string folder);