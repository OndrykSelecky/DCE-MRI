#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"

const int OPTIMAL_TRIANGULATION = 1;
const int HOMOGRAPHY = 2;

features detect_features(const MRISequence& sequence, double max_difference = 5.0f, int maxCorners = 500, double qualityLevel = 0.001, int minDistance = 10);

void show_features(MRISequence& sequence, const features& features);

std::shared_ptr<MRISequence> registration(MRISequence& sequence, const int method = OPTIMAL_TRIANGULATION);

MRISequence warp_sequence(const MRISequence& sequence, const features& features);




