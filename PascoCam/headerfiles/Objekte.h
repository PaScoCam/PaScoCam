#include "Include.h"

using namespace std;
using namespace cv;

class Objekte{
 public:
	Objekte(double verschiebung);
	vector<KeyPoint> keypointObjekte;
	long count;
	double verschiebung;
	double summe;
	double xLo,yLo,xHi,yHi,xC,yC;
};