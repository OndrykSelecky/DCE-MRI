#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

class MyOpenCVDicomReader
{
public:
	cv::Mat GetImage(const std::string& file_name);

};
