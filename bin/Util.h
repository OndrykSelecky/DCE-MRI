#pragma once
#include <vector>
#include "opencv2/opencv.hpp"

typedef std::vector<std::vector<cv::Point2f>> features;


std::vector<std::string> get_file_names(std::string folder, const std::string& suffix = "");
std::vector<std::string> get_folder_names(std::string folder);