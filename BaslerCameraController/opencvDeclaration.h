#pragma once

#include "declaration.h"

#include <opencv2/opencv.hpp>

using namespace cv;

namespace cv_func {
	void saveImage(string filename, const void* data, size_t width, size_t height, int pixelType = CV_8UC1);
	void saveImage(string filename, Mat openCvImage);
}

//void saveImage(string filename, const void* data, size_t width, size_t height, int pixelType = CV_8UC1);
//void saveImage(string filename, Mat openCvImage);