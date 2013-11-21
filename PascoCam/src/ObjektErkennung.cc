#include "ObjektErkennung.h"

using namespace std;
using namespace cv;

ObjektErkennung::ObjektErkennung(int hesseSchwellwert, bool upright, bool extended, double min_distV , double max_distV ,double dYMax){
	hesseSchwell = hesseSchwellwert;
	upr = upright;
	ext = extended;
	min_distV = min_distV;
	max_distV = max_distV;
	dYMax = dYMax;
	detector.hessianThreshold = hesseSchwellwert;
	detector.upright = upright;
	detector.extended = extended;
};

void ObjektErkennung::erkenneObjekte(){


	// Hier alle zeitaufwendigen und notwendigen Berechnungen durchführen
	double t1 = (double)cvGetTickCount();
	//-- Schritt 1: Erkenne keypoints
//		detector.detect( bildM1, keypointsM1 );
	detector.detect( bildAktuell, keypointsAktuell );

	double t2 = (double)cvGetTickCount();
	//-- Schritt 2: Berechne Deskriptoren
//		extractor.compute( bildM1, keypointsM1, deskriptorM1 );
	extractor.compute( bildAktuell, keypointsAktuell, deskriptorAktuell );

	double t3 = (double)cvGetTickCount();
	//-- Schritt 3: Match Deskriptoren mit FLANN matcher
	matcher.match( deskriptorM1, deskriptorAktuell, matches );

	double t4 = (double)cvGetTickCount();
	//-- Schritt 4: Bestimmung guter Matches
	bestimmeGuteMatches();

	double t5 = (double)cvGetTickCount();
	//-- Schritt 5: Punkte zu Objekten zusammenfassen
	bestimmeObjekte();

	double t6 = (double)cvGetTickCount();
	//-- Schritt 6: Bewegung von Objekten bestimmen



	t1 = t2 - t1;
	t2 = t3 - t2;
	t3 = t4 - t3;
	t4 = t5 - t4;
	t5 = t6 - t5;
	
	printf( "Berechnungszeit für die Keypoints = %gms\n", t1/(cvGetTickFrequency()*1000.));
	printf( "Berechnungszeit für die Deskriptoren = %gms\n", t2/(cvGetTickFrequency()*1000.));
	printf( "Berechnungszeit für die Matches = %gms\n", t3/(cvGetTickFrequency()*1000.));
	printf( "Berechnungszeit für die guten Matches = %gms\n", t4/(cvGetTickFrequency()*1000.));
	printf( "Berechnungszeit für die Objektbestimmung = %gms\n", t5/(cvGetTickFrequency()*1000.));
	
	/*
	for( int i = 0; i < (int)good_matches.size(); i++ ){ 
		cout<< "Verschiebung		: "<< keypointsAktuell.at(good_matches[i].trainIdx).pt.x-keypointsM1.at(good_matches[i].queryIdx).pt.x << endl;	
	}*/
		
		

	//printf("-- Max dist : %f \n", max_dist );
	//printf("-- Min dist : %f \n", min_dist );
	cout<< "Matches		: "<< matches.size() << endl;
	cout<< "Gute Matches	: "<< good_matches.size() << endl;


	if (true){
		//-- Zeichne "gute" Matches
		drawMatches( bildM1, keypointsM1, bildAktuell, keypointsAktuell,
				good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	}else{
		//-- Zeichne alle Matches
		drawMatches( bildM1, keypointsM1, bildAktuell, keypointsAktuell,
				matches, img_matches, Scalar::all(-1), Scalar::all(-1),
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	} 

	

	for(int j=0;j<(int)erkannteObjekte.size();j++){
		cout<<"obj "<<j<<" Anzahl "<<erkannteObjekte[j].count<<" x " << erkannteObjekte[j].xHi<<" Versch "<<erkannteObjekte[j].verschiebung<<endl;
/*		for(int i=0;i<(int)erkannteObjekte[j].keypointObjekte.size();i++){
			cout<<j<<" "<<i<<"X "<<erkannteObjekte[j].keypointObjekte[i].pt.x<< "  Y " <<erkannteObjekte[j].keypointObjekte[i].pt.y<<endl;
		}*/
	
		CvScalar s;
		s.val[0]=0;
		s.val[1]=0;
		s.val[2]=255;
		drawKeypoints(bildKeypoints,erkannteObjekte[j].keypointObjekte,bildKeypoints,s,0);
	}
}

void ObjektErkennung::bestimmeGuteMatches(){
	if(false){
		//Bestimmung guter Matches mit Hilfe kleiner Abstände

		double max_dist=max_distV; double min_dist= min_distV;
		//-- Quick calculation of max and min distances between keypoints
		for( int i = 0; i < (deskriptorM1).rows; i++ )
		{ 
			double dist = matches[i].distance;
				
			if( dist < min_dist ) min_dist = dist;
			if( dist > max_dist ) max_dist = dist;
		}
		for( int i = 0; i < (deskriptorM1).rows; i++ )
		{ 
			if( matches[i].distance < 2*min_dist ){ 
				good_matches.push_back( matches[i]); 
			}
		}
	}else{
		//Bestimmung guter Matches mit Hilfe kleiner Abweichungen in y
		/*
			Mit Hilfe dieses Verfahrens werden fast nur gute Matches gefunden, während die Berechnungszeit durch
			einen deutich größeren Hesseschwellwert deutlich verkürz werden kann. Ohne das die Qualität der gefunden
			Matches deutlich nachlässt.

		*/
		for( int i = 0; i < (deskriptorM1).rows; i++ )
		{ 
			// keypointsAktuell.at(matches[i].trainIdx).pt.y-keypointsM1.at(matches[i].queryIdx).pt.y;
			// keypoints dürfen sich nur wenig in der Höhe ändern, und die Bewegung in x muss negativ sein
			if( abs(keypointsAktuell.at(matches[i].trainIdx).pt.y-keypointsM1.at(matches[i].queryIdx).pt.y)<dYMax){ 
				if((keypointsAktuell.at(matches[i].trainIdx).pt.x-keypointsM1.at(matches[i].queryIdx).pt.x)<0){
					good_matches.push_back( matches[i]); 
				}
			}
		}
	}
}

void ObjektErkennung::gruppiereKeyPoints(){
	// Keypunkte werden nach der Bewegung in Grupen eingeteilt.
// Problem: Keypunkte in gleicher Entfernung werden in die gleiche Gruppe einsortiert.
	if (good_matches.size()>0){
		// Einfügen erstes Objekt
		if(objekte.size()<1){
			objekte.push_back(Objekte(keypointsAktuell.at(good_matches[0].trainIdx).pt.x-keypointsM1.at(good_matches[0].queryIdx).pt.x));
			objekte[0].count=1;
			objekte[0].keypointObjekte.push_back(keypointsAktuell.at(good_matches[0].trainIdx));
		}

		// Durchlaufe alle Matches und trage sie in Objekte ein
		for( int i = 1; i < (int)good_matches.size(); i++ ){ 
			bool neuesObjekt=true;
			double aktuelleBewegung = keypointsAktuell.at(good_matches[i].trainIdx).pt.x-keypointsM1.at(good_matches[i].queryIdx).pt.x;	
			
			for(int j=0;j<(int)objekte.size();j++){
				// wenn die prozentuale Verschiebung gleich ist.
				if(abs((aktuelleBewegung-objekte[j].verschiebung)/aktuelleBewegung)<abweichung){
					// Berechnug Mittelwert der Verschiebung
					//objekte[j].verschiebung=(objekte[j].verschiebung+aktuelleBewegung);
					objekte[j].summe=(objekte[j].summe+aktuelleBewegung);
					objekte[j].count++;
					objekte[j].keypointObjekte.push_back(keypointsAktuell.at(good_matches[i].trainIdx));
					objekte[j].verschiebung=objekte[j].summe/objekte[j].count;
					neuesObjekt=false;
				}
			}
			if(neuesObjekt){
				objekte.push_back(Objekte(aktuelleBewegung));
				objekte[objekte.size()-1].keypointObjekte.push_back(keypointsAktuell.at(good_matches[i].trainIdx));
				objekte[objekte.size()-1].count=1;
			}

		}
	}



	;

	// Zusammenfügen von Objekten in gleicher Entfernung in einen Vector
	for(int j=0;j<(int)objekte.size();j++){
		if(objekte[j].count>1){
			if(objekte[j].count/((double)good_matches.size())>prozentsatz){
				gleicheEntfernungObj.push_back(objekte[j]);
			}
		}
	}


}

void ObjektErkennung::objekteZusammenfassen(){
	// Objekte zusammenfügen, wenn sie beim gleichen x sind und sich gleich weit bewegt haben.
	// Ist der Fall, wenn Anfangspunkte weit auseinander sind und sich dann durch weitere Keypunkte nähern.

	// Berechnung xC verbessern
	for(int i=0;i<(int)objekte.size();i++){
		double xc=0,yc=0;
		double xQuer,yQuer;
		objekte[i].xC=(objekte[i].xHi-objekte[i].xLo)/2 +objekte[i].xLo;
		for(int j=0;j<(int)objekte[i].keypointObjekte.size();j++){
			//xc berechnen
			int anzahl=0;
			if(objekte[i].keypointObjekte[j].pt.x>objekte[i].xLo&&objekte[i].keypointObjekte[j].pt.x<objekte[i].xHi){
				xQuer+=objekte[i].keypointObjekte[j].pt.x;
				anzahl++;
			}
			if(anzahl>0){
				xQuer=xQuer/anzahl;
			}

			//yc berechnen
			anzahl=0;
			if(objekte[i].keypointObjekte[j].pt.y>objekte[i].yLo&&objekte[i].keypointObjekte[j].pt.y<objekte[i].yHi){
				yQuer+=objekte[i].keypointObjekte[j].pt.y;
				anzahl++;
			}
			if(anzahl>0){
				yQuer=yQuer/anzahl;
			}
		}

		// Falls zu wenige Werte vorhanden waren 
		if(xc==0) {
			objekte[i].xC=(objekte[i].xHi-objekte[i].xLo)/2 +objekte[i].xLo;
		}
		if(yc==0) {
			objekte[i].yC=(objekte[i].yHi-objekte[i].yLo)/2 +objekte[i].yLo;
		}
		objekte[i].xC=xc;
		objekte[i].yC=yc;
	}

	for(int i=0;i<(int)objekte.size();i++){
		for(int j=0;j<i;j++){
			if(abs(objekte[i].verschiebung-objekte[j].verschiebung)<10){ 
				if(abs(objekte[i].xC-objekte[j].xC)<50&&objekte[j].count>0){
					for(int k=0;k<(int)objekte[j].keypointObjekte.size();k++){
						objekte[i].keypointObjekte.push_back(objekte[j].keypointObjekte[k]);
					}
					objekte[j].count=-2;
				}
			}
		}
	}

	for(int i=0;i<(int)objekte.size();i++){
		if(objekte[i].count>0){
			gleicheEntfernungObj.push_back(objekte[i]);
		}
	}
}

void ObjektErkennung::histogrammErstellenUndAuswerten(){
	int unterteilung=6;
	

// Trennen von gleichen Entfernungen mit großem Abstand in 2 Objekte
// Unterteilen von xLo bis xHi in 6 Bereiche, ein Histogramm über die Bereiche erstellen
// sobald ein Bereich einen Schwellwert unterschreitet, fängt dort ein neues Objekt an.

	 
	// Erstellung der benötigten Histogramme
	for(int i=0;i<(int)gleicheEntfernungObj.size();i++){
		vector<int> hist(unterteilung);
		vector<int> histBereinigt(unterteilung);
		int dx=(int)(gleicheEntfernungObj[i].xHi-gleicheEntfernungObj[i].xLo)/unterteilung;

		for(int l=0;l<unterteilung;l++){
			hist[l]=0;
			for(int k=0;k<(int)gleicheEntfernungObj[i].keypointObjekte.size();k++) {
				//befindet sich der Punkt im Intervall
				if(gleicheEntfernungObj[i].keypointObjekte[k].pt.x<=(l+1)*dx+gleicheEntfernungObj[i].xLo+3&&gleicheEntfernungObj[i].keypointObjekte[k].pt.x>=l*dx+gleicheEntfernungObj[i].xLo-3){
					hist[l]++;
				}
			}
		}
		bool neuesObjekt=true;
		// Bereinigung des Histogrammes, dass für jedes Objekt eine Zahl mit dem Wert des Histogrammes aufgeführt wird
		double sum=0;
		for(int j=0;j<unterteilung;j++){
			sum+=hist[j];
		}

		// sobald zuerst weniger und dann mehr als 4 Prozent in einem Bereich liegen wird er als neuer Bereich interpretiert
		for(int l=0;l<unterteilung;l++){
			int anteil=(int)(100*hist[l]/sum);

			if(anteil>4&&neuesObjekt==true){
				histBereinigt[l]=anteil;
				neuesObjekt=false;
			}
			if(anteil<=4){
				neuesObjekt=true;
			}
		}
		// Bestimmung wie viele Bereiche ein Intervall hat


		// Gibt den Anteil ein, den Einteilbereich an einem zuvor als ein Bereich erkannten
		// Objekte haben muss.
		int anteilAlsBereich=20;

		sum=0;
		int anzGrSchw=0;
		int ddx=0;
		for(int j=0;j<unterteilung;j++){
			sum+=histBereinigt[j];
			if((int)(100*histBereinigt[j]/sum>anteilAlsBereich)){
				anzGrSchw++;
			}
		}
		int countUnterteilung=0;
		// Schleife für jeden Unterteilungsschritt
		for(int j=0;j<unterteilung;j++){
			int anteil=(int)(100*histBereinigt[j]/sum);
			// Es wird überprüft, ob der Bereich als neuer Bereich eingestuft werden kann
			if(anteil>anteilAlsBereich){
				vector<KeyPoint> keyPunkteZumEintragen;
				int countZumEintragen=0;


				ddx=(int)(gleicheEntfernungObj[i].xHi-gleicheEntfernungObj[i].xLo)/anzGrSchw;

				int objNr=0;
				
				for(int k=0; k<(int)gleicheEntfernungObj[i].keypointObjekte.size();k++){
					// hier wird überprüft, in welchem der letzten anzGrSchw objekte der keypoint eingetragen wird.
					if(gleicheEntfernungObj[i].keypointObjekte[k].pt.x>countUnterteilung*ddx+gleicheEntfernungObj[i].xLo-0&&gleicheEntfernungObj[i].keypointObjekte[k].pt.x<(countUnterteilung+1)*ddx+gleicheEntfernungObj[i].xLo+0){
						keyPunkteZumEintragen.push_back(gleicheEntfernungObj[i].keypointObjekte[k]);
						countZumEintragen++;
					}
				}

				if(countZumEintragen>0){
					erkannteObjekte.push_back(Objekte(gleicheEntfernungObj[i].verschiebung));
					erkannteObjekte[erkannteObjekte.size()-1].keypointObjekte=keyPunkteZumEintragen;
					erkannteObjekte[erkannteObjekte.size()-1].count=countZumEintragen;
				}

				countUnterteilung++;
			}else{
				if(anteil>0){
					countUnterteilung++;
				}
			}	
		}
	}

}

void ObjektErkennung::bestimmeObjekte(){
	// erlaubte Abweichung der Verschiebung
	abweichung=0.04; //
	// Prozentsatz, welcher Teil der keypoints eines Objekts nötig sind, um ein
	// neues Objekt zu definieren
	prozentsatz=0.15; //
	objekte.clear();
	gleicheEntfernungObj.clear();

	
	double t1 = (double)cvGetTickCount();
	// die Keypoints in Gruppen zusammenfassen.
	gruppiereKeyPoints();

	
	objekte=gleicheEntfernungObj;
	gleicheEntfernungObj.clear();

	double t2 = (double)cvGetTickCount();
	// Objekte am selben Ort und mit gleicher Geschwindigkeit zu einem Objekt zusammenfügen.
	objekteZusammenfassen();

	double t3 = (double)cvGetTickCount();
	gleicheEntfernungObj=bestimmeObjektGrosse(gleicheEntfernungObj); 

	double t4 = (double)cvGetTickCount();
	// Histogramme auswerten, um Objekte mit gleicher Geschwindigkeit, die weit auseinander sind zu trennen.
	histogrammErstellenUndAuswerten();
//	erkannteObjekte=gleicheEntfernungObj;
	double t5 = (double)cvGetTickCount();


	t1 = t2 - t1;
	t2 = t3 - t2;
	t3 = t4 - t3;
	t4 = t5 - t4;
/*
	printf( "Berechnungszeit für Gruppieren = %gms\n", t1/(cvGetTickFrequency()*1000.));
	printf( "Berechnungszeit für Zusammenfassen = %gms\n", t2/(cvGetTickFrequency()*1000.));
	printf( "Berechnungszeit für Objektgrose = %gms\n", t3/(cvGetTickFrequency()*1000.));
	printf( "Berechnungszeit für Histogramm = %gms\n", t4/(cvGetTickFrequency()*1000.));
	*/
	erkannteObjekte=bestimmeObjektGrosse(erkannteObjekte);
}

vector<Objekte> ObjektErkennung::bestimmeObjektGrosse(vector<Objekte> zuPrufendeObjekte){
	for(int i=0;i<(int)zuPrufendeObjekte.size();i++){
		if(zuPrufendeObjekte[i].keypointObjekte.size()>0){
			if(zuPrufendeObjekte[i].xLo=-1){
				zuPrufendeObjekte[i].xLo=zuPrufendeObjekte[i].keypointObjekte[0].pt.x;
				zuPrufendeObjekte[i].xHi=zuPrufendeObjekte[i].keypointObjekte[0].pt.x;
			}

			if(zuPrufendeObjekte[i].yLo=-1){
				zuPrufendeObjekte[i].yLo=zuPrufendeObjekte[i].keypointObjekte[0].pt.y;
				zuPrufendeObjekte[i].yHi=zuPrufendeObjekte[i].keypointObjekte[0].pt.y;
			}

			for(int j=1;j<(int)zuPrufendeObjekte[i].keypointObjekte.size();j++){
				//xLo bestimmen
				if(zuPrufendeObjekte[i].xLo>zuPrufendeObjekte[i].keypointObjekte[j].pt.x){
					zuPrufendeObjekte[i].xLo=zuPrufendeObjekte[i].keypointObjekte[j].pt.x;
				}

				//yLo bestimmen
				if(zuPrufendeObjekte[i].yLo>zuPrufendeObjekte[i].keypointObjekte[j].pt.y){
					zuPrufendeObjekte[i].yLo=zuPrufendeObjekte[i].keypointObjekte[j].pt.y;
				}

				//xLo bestimmen
				if(zuPrufendeObjekte[i].xHi<zuPrufendeObjekte[i].keypointObjekte[j].pt.x){
					zuPrufendeObjekte[i].xHi=zuPrufendeObjekte[i].keypointObjekte[j].pt.x;
				}

				//yLo bestimmen
				if(zuPrufendeObjekte[i].yHi<zuPrufendeObjekte[i].keypointObjekte[j].pt.y){
					zuPrufendeObjekte[i].yHi=zuPrufendeObjekte[i].keypointObjekte[j].pt.y;
				}
			}
		}
	}
	return zuPrufendeObjekte;
}

//! 
/*! Paint the parallel bounding box
*/
void ObjektErkennung::paintBoundingBox (){
	//(*meineObjektErkennung).
	for(int i=0;i<(int)erkannteObjekte.size();i++){
		CvPoint p1=(CvPoint)Point((int)erkannteObjekte[i].xLo-1,(int)erkannteObjekte[i].yLo-1);
		CvPoint p2=(CvPoint)Point((int)erkannteObjekte[i].xLo-1,(int)erkannteObjekte[i].yHi+1);
		CvPoint p3=(CvPoint)Point((int)erkannteObjekte[i].xHi+1,(int)erkannteObjekte[i].yLo-1);
		CvPoint p4=(CvPoint)Point((int)erkannteObjekte[i].xHi+1,(int)erkannteObjekte[i].yHi+1);
		CvScalar s;
		s.val[0]=255;
		s.val[1]=0;
		s.val[2]=0;
		line(bildKeypoints,p1,p2,s,2);
		line(bildKeypoints,p2,p4,s,2);
		line(bildKeypoints,p4,p3,s,2);
		line(bildKeypoints,p3,p1,s,2);
	}
}

