#include "Include.h"

using namespace std;
using namespace cv;

class TimeMeasurement{
protected:
	int64 timestamp_before;
	int64 timestamp_after;
	double time;

public:
	TimeMeasurement();
	void setTimestamp_before(int64 timestamp);
	void setTimeStamp_after(int64 timestamp);
	double calculateTime();
};