#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"


//sequence_features detect_features(const MRISequence& sequence);

feat detect_features_backwards(const MRISequence& sequence, double max_difference = 1.5f, int maxCorners = 500, double qualityLevel = 0.01, int minDistance = 5);

void show_features(MRISequence& sequence, const feat& features);

void registration(const std::string& folder, int sequence_id);

MRISequence warp_sequence(const MRISequence& sequence, const feat& features);

//void motion_test();



