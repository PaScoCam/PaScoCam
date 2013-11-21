#include "ImageProperties.h"
#include "TimeMeasurement.h"
#include "ReadImage.h"

using namespace std;
using namespace cv;

ReadImage::ReadImage(){ 
	kameraIndex = 0;
	ungueltigeFrames = 0;
	// Objekterzeugung
	VideoCapture camera(kameraIndex);
};

ReadImage::ReadImage(int kameraIndex, int ungueltigeFrames){
	kameraIndex = kameraIndex;
	ungueltigeFrames = ungueltigeFrames;
};

void ReadImage::bildParameterZuweisen(){
	parameter.push_back(CV_IMWRITE_JPEG_QUALITY);
	parameter.push_back(100);
}

void ReadImage::vectorizeImages(Mat image){
	images.push_back(image.clone());
}

void ReadImage::saveImages(){
	for(int index = 0; index < (int)images.size(); index++){
		stringstream lfdnr;
		lfdnr << index;
		string bildname = "images/workspace/image"+lfdnr.str()+".png";
		imwrite(bildname, images[index]);
	}
}

Mat ReadImage::read(){
	VideoCapture camera(kameraIndex);
	// Abfrage des bereits geöffneten Streams
	if(!camera.isOpened()){
		cout << "Keine Kamera unter angegebenem Index erkannt." << endl;
	}

	// Initialization of the picture
	// also width and height of the video stream
	Mat image;

	/*
		declarate the timestamps for measuring the time needed to take a picture either
		from a constant stream or from a stream whose object is initialized every time again in the 
		for-loop. For saving the resulting time stamps vector time is used and cleared every time the 
		function bildEinlesen() is called.
	*/
	int anzahl = 0;
	vector<double> time;
	vector<ImageProperties> properties;
	TimeMeasurement measure;

	
	// Erstes Bild aussortieren (Belichtungs- und Zeitpunktkompensierung).
	// function read returns *last* picture of the stream, not the current one
	// two times reading returns the current picture of the stream
	camera.read(image);

	// show the image & the specific number
	// return also the current frame rate of the webcam

	// get the tickCount() before the image is taken
	measure.setTimestamp_before(getTickCount());

	if(camera.read(image)){
		anzahl++;
		imshow("AufgenommenerFrame", image);
					
		vectorizeImages(image);

		// get the TickCount() after image is taken and calculate the time needed
		measure.setTimeStamp_after(getTickCount());
		time.push_back(measure.calculateTime());
		
		cout << CV_CAP_PROP_FRAME_HEIGHT << endl;
		properties.push_back(ImageProperties(
			camera.get(CV_CAP_PROP_FRAME_WIDTH),
			camera.get(CV_CAP_PROP_FRAME_HEIGHT),
			camera.get(CV_CAP_PROP_HUE)));

		cout << "Bild #" << anzahl << " aufgenommen und ausgegeben" << endl;
		cout << "Bildparameter: " << "\n"
				<< "\t Bildbreite: " << properties[anzahl-1].getWidth() << " Pixel.\n" 
				<<	"\t Bildh\x94he: " << properties[anzahl-1].getHeight() << " Pixel.\n"
				<< "\t Kontrast: " << properties[anzahl-1].getContrast() << "\n" << endl;
		cout << endl;
	}

	/*if(GetAsyncKeyState(VK_UP)){
		// Calculate the mean time value of the timestamps of each call *.read(Mat& img)
		double meanTime = 0;
		for(int j = 0; j < (int)time.size(); j++){
			meanTime += time[j];
		}
		meanTime /= time.size();
		cout << "Dauer der Aufnahme und Ausgabe nach " << anzahl << " Aufnahmen: " << meanTime << " msec." << endl;
		saveImages();
	}
	if(GetAsyncKeyState(VK_END)){
		break;
	}*/
	Mat gray_image;
	cvtColor( image, gray_image, CV_BGR2GRAY );
	return gray_image;
}