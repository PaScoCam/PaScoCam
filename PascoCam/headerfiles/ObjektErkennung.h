#include "Include.h"
#include "Objekte.h"

using namespace std;
using namespace cv;

class ObjektErkennung{
public:
	// Featureerkennung
	int hesseSchwell;
	//für Stereovision ohne Beachtung des Winkels
	bool upr;
	// 0=64 Bit, 1=128 Bit
	bool ext;

	// Matcher
	// Für kleine Abstände
	double max_distV; double min_distV;

	// Für kleine y Verschiebung
	double dYMax;


	vector<Objekte> objekte;
	vector<Objekte> gleicheEntfernungObj;
	// Prozentsatz, um den sich die Verschiebung unterscheiden darf
	double abweichung; //1=100%
	// Prozentsatz, welcher Teil der Matches für ein Objekt steht
	double prozentsatz; //1=100%
	


	//Initialisierung
	Mat bildAktuell;
	Mat bildM1;
	Mat bildKeypoints;
	SurfFeatureDetector detector;

	std::vector<KeyPoint> keypointsM1, keypointsAktuell;
	SurfDescriptorExtractor extractor;
	Mat deskriptorM1, deskriptorAktuell;
	FlannBasedMatcher matcher;
	Mat img_matches;

	//alle Matches
	std::vector< DMatch > matches;
	//gute matches (deren Abstand weniger als 2*min_dist ist )
	std::vector< DMatch > good_matches;

	vector<Objekte> erkannteObjekte;
	ObjektErkennung(int hesseSchwellwert, bool upright, bool extended, double min_distV , double max_distV ,double dYMax);

	//Functions
	void erkenneObjekte();
	void bestimmeGuteMatches();
	void bestimmeObjekte();
	vector<Objekte> bestimmeObjektGrosse(vector<Objekte> zuPrufendeObjekte);
	void paintBoundingBox ();
	void gruppiereKeyPoints();
	void objekteZusammenfassen();
	void histogrammErstellenUndAuswerten();
};