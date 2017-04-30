#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"


//Returns features detected throughout the sequence
features detect_features(const MRISequence& sequence, double max_distance = 5.0f, int max_corners = 500, double quality_level = 0.001, int min_distance = 10);


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
MRISequence registration(MRISequence& sequence, features& features, const int method = OPTIMAL_TRIANGULATION, bool show = false);






