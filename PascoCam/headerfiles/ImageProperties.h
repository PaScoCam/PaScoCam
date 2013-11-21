#include "Include.h"

using namespace std;
using namespace cv;

class ImageProperties{
protected:
	double width;
	double height;
	double contrast;

public:
	ImageProperties(double width, double height, double contrast);
	double getWidth();
	double getHeight();
	double getContrast();
};