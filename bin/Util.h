#pragma once
#include <vector>
#include "opencv2/opencv.hpp"
#include "s_hull_pro.h"

/*
definition of constants and function for file reading
*/

//codes for entries in dicom header
const std::string SEQUENCE_ENTRY_ID = "0020|0011";
const std::string IMAGE_ENTRY_ID = "0020|0013";


//names of transformation types
const int OPTIMAL_TRIANGULATION = 1;
const int HOMOGRAPHY = 2;


//name of files, where acquisition numbers of images and sequences are stored
const std::string IMAGE_ORDER_FILE = "images.txt";
const std::string SEQUENCE_ORDER_FILE = "sequences.txt";


//data structure for features
typedef std::vector<std::vector<cv::Point2f>> features;


//structure for representing a triangle
struct Triangle
{
	//id-s of points in features vector
	int a;
	int b;
	int c;

	//coordinates of these points (copied)
	std::vector<cv::Point2f> point_coordinates;
};


//returns names of all files with suffix in folder
std::vector<std::string> get_file_names(std::string folder, const std::string& suffix = "");


//returns names of subfolders in folder
std::vector<std::string> get_folder_names(std::string folder);


//get name of first file with suffix in folder
std::string get_first_file_name(const std::string& folder, const std::string& suffix = "");


//Point comparisions functions from s-hull algorithm, part of s-hull implementation
bool pointSortPredicate(const Shx& a, const Shx& b);

bool pointComparisonPredicate(const Shx& a, const Shx& b);