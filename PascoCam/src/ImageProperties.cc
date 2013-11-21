#include "ImageProperties.h"
#include "TimeMeasurement.h"
#include "ReadImage.h"

using namespace std;
using namespace cv;

ImageProperties::ImageProperties(double width, double height, double contrast){
	width = width;
	height = height;
	contrast = contrast;
}

double ImageProperties::getWidth(){
	return width;
}

double ImageProperties::getHeight(){
	return height;
}

double ImageProperties::getContrast(){
	return contrast;
}