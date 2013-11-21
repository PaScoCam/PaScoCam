#include "TimeMeasurement.h"
#include "ImageProperties.h"
#include "ReadImage.h"

using namespace std;
using namespace cv;

TimeMeasurement::TimeMeasurement(){
	timestamp_before = 0;
	timestamp_after = 0;
}

void TimeMeasurement::setTimestamp_before(int64 timestamp){
	timestamp_before = timestamp;
}

void TimeMeasurement::setTimeStamp_after(int64 timestamp){
	timestamp_after = timestamp;
}

double TimeMeasurement::calculateTime(){
	time = (timestamp_after - timestamp_before) / getTickFrequency();
	return time;
}
