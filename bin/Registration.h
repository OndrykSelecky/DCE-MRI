#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "MRISequence.h"
#include "Util.h"


features detect_features(const MRISequence& sequence);

void show_features(MRISequence& sequence, const features& features);

void registration();

MRISequence registrate(const MRISequence& sequence, const features& features);

//void motion_test();



