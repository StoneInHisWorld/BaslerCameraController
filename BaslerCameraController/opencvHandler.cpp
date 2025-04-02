#include "opencvDeclaration.h"
#include "pathDeclaration.h"

void cv_func::saveImage(string filename, const void* data, size_t width, size_t height, int pixelType)
{
	// Create an OpenCV image from a pylon image.
	Mat openCvImage = Mat(height, width, pixelType, (uint8_t*)data);
	// 检查目录是否存在
	fs::path p = get_directory(filename);
	if (!fs::exists(p))
	{
		fs::create_directories(p);
	}
	// Save the image using OpenCV.
	imwrite(filename, openCvImage);
	//cout << "Image saved at: " << filename << endl;
}

void cv_func::saveImage(string filename, Mat openCvImage)
{
	// 检查目录是否存在
	fs::path p = get_directory(filename);
	if (!fs::exists(p))
	{
		fs::create_directories(p);
	}
	// Save the image using OpenCV.
	imwrite(filename, openCvImage);
	//cout << "Image saved at: " << filename << endl;
}