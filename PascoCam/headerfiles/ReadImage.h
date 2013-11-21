#include "Include.h"

using namespace std;
using namespace cv;

class ReadImage{
protected:
	//VideoCapture camera;
	vector<int> parameter;
	void bildParameterZuweisen();
	void vectorizeImages(Mat image);
	void saveImages();

public:
	int kameraIndex;
	int ungueltigeFrames;
	vector<Mat> images;
	ReadImage();
	ReadImage(int kameraIndex, int ungueltigeFrames);
	Mat read();
};
