#pragma once
#include <vector>
#include "opencv2/opencv.hpp"
#include "s_hull_pro.h"

const std::string SEQUENCE_ENTRY_ID = "0020|0011";
const std::string IMAGE_ENTRY_ID = "0020|0013";

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
std::string get_first_file_name(const std::string& folder, const std::string& suffix = "");

//Point comparisions functions from s-hull algorithm
bool pointSortPredicate(const Shx& a, const Shx& b);

bool pointComparisonPredicate(const Shx& a, const Shx& b);