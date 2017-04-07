#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"

const int OPTIMAL_TRIANGULATION = 1;
const int HOMOGRAPHY = 2;

typedef std::vector<std::vector<cv::Point2f>> features;



//Returns features detected throughout the sequence
features detect_features(const MRISequence& sequence, double max_difference = 5.0f, int maxCorners = 500, double qualityLevel = 0.001, int minDistance = 10);


//Finds triangulation of all images in sequence
std::vector<Triangle> triangulate(features& features, const MRISequence& sequence);


//Global warping
MRISequence warp_sequence(const MRISequence& sequence, const features& features);


//Local warping using optimal triangulation
MRISequence warp_sequence(const MRISequence& sequence, const features& features, const std::vector<Triangle>& triangles);


//Show sequence with detected features
void show_features(MRISequence& sequence, const features& features);


//Show triangulated sequence
void show_triangles(MRISequence& sequence, const std::vector<Triangle>& triangles, const features& features);


//Main registration function
std::shared_ptr<MRISequence> registration(MRISequence& sequence, const int method = OPTIMAL_TRIANGULATION, bool reverse = false, bool show = false);






