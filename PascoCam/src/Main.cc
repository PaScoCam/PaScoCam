#include "Berechnung.h"
#include "ImageProperties.h"
#include "ObjektErkennung.h"
#include "ReadImage.h"
#include "TimeMeasurement.h"

using namespace std;
using namespace cv;

int main(int argc, const char** argv){

	int hesseSchwellwert = 350;
	
	//für Stereovision ohne Beachtung des Winkels
	bool upright=true;
	
	// 0=64 Bit, 1=128 Bit
	bool extended=true;

	// Matcher
	// Für kleine Abstände
	double max_distV = 0; double min_distV = 100;

	// Für kleine y Verschiebung
	double dYMax = 5;

	ObjektErkennung meineObjektErkennung(hesseSchwellwert,upright,extended,max_distV,min_distV,dYMax);
	// Berechnung berechne;
	ReadImage capture;

	while(true){
		//reset the vectors for the matched objects / descriptors
		meineObjektErkennung.good_matches.clear();
		meineObjektErkennung.erkannteObjekte.clear();

		// set current as old etc..
		meineObjektErkennung.bildM1=meineObjektErkennung.bildAktuell;
		meineObjektErkennung.deskriptorM1=meineObjektErkennung.deskriptorAktuell;
		meineObjektErkennung.keypointsM1=meineObjektErkennung.keypointsAktuell;

		// load the image as a greyScale
		meineObjektErkennung.bildAktuell = capture.read();
		meineObjektErkennung.bildKeypoints=meineObjektErkennung.bildAktuell;

		// Anfrage Plausibilität
		if(!(meineObjektErkennung.bildAktuell).data )
		{ std::cout<< " --(!) Error reading images " << std::endl; return -1; }

		// object detection
		meineObjektErkennung.erkenneObjekte();

		// visualize detected objects
		meineObjektErkennung.paintBoundingBox();

		//-- Zeige erkannte matches
		if(meineObjektErkennung.img_matches.rows>0)
			imshow( "Good Matches", meineObjektErkennung.img_matches );
		if(meineObjektErkennung.bildKeypoints.rows>0)
			imshow( "keypoints", meineObjektErkennung.bildKeypoints );

		if (false){
			for( int i = 0; i < (int)meineObjektErkennung.good_matches.size(); i++ )
			{ printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, meineObjektErkennung.good_matches[i].queryIdx, meineObjektErkennung.good_matches[i].trainIdx ); }
		}

		cout<<"    "<<endl;
		waitKey(0);
	}
	return 0;
}