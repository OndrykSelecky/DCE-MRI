#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"


features detect_features(const MRISequence& sequence, double max_difference = 5.0f, int maxCorners = 500, double qualityLevel = 0.001, int minDistance = 7);

void show_features(MRISequence& sequence, const features& features);

void registration(const std::string& folder, int sequence_id);

MRISequence warp_sequence(const MRISequence& sequence, const features& features);

//void motion_test();



