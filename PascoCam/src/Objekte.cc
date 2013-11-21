#include "Objekte.h"

using namespace std;
using namespace cv;

Objekte::Objekte(double verschiebung){
	verschiebung = verschiebung; 
	summe = verschiebung;
	xLo=-1;
	yLo=-1;
	xHi=-1;
	yHi=-1;
	count = 0;
}  
