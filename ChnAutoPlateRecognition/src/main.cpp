/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
*****************************************************************************/

// Main entry code OpenCV

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <opencv/ml.h>

#include <iostream>
#include <vector>

#include "DetectRegions.h"
#include "OCR.h"

using namespace std;
using namespace cv;

string getFilename(string s) {

    char sep = '/';
    char sepExt='.';

    #ifdef _WIN32
        sep = '\\';
    #endif

    size_t i = s.rfind(sep, s.length( ));
    if (i != string::npos) {
        string fn= (s.substr(i+1, s.length( ) - i));
        size_t j = fn.rfind(sepExt, fn.length( ));
        if (i != string::npos) {
            return fn.substr(0,j);
        }else{
            return fn;
        }
    }else{
        return "";
    }
}

//Main entry
int main ( int argc, char** argv )
{
    cout << "Automatic Number Plate Recognition of China\n";
    char* filename;
    char* strCmdOpt;
    bool bSegmentPlateOnly = false;
    bool bSegmentCharOnly = false;
    bool bShowSteps = false;
    Mat input_image;

    //Check if user specify image to process
    if(argc == 2 ) {
        filename = argv[1];
    } else if(argc == 3) {
    	strCmdOpt = argv[1];
    	filename = argv[2];

    	if((strchr(strCmdOpt, '-')) && (strchr(strCmdOpt, 's')))
    		bSegmentPlateOnly = true;

    	if((strchr(strCmdOpt, '-')) && (strchr(strCmdOpt, 'c')))
    		bSegmentCharOnly = true;

    	if((strchr(strCmdOpt, '-')) && (strchr(strCmdOpt, 't')))
    		bShowSteps = true;
    } else {
        printf("Usage:\t%s [-s][c][t] image_file\n", argv[0]);
        printf("\t-s: ONLY Segment the plates from image and save to ../tmp\n");
        printf("\t-c: Segment the char from plate and save to ../tmpChar\n");
        printf("\t-t: Show each processing steps\n\n");
        return 0;
    }        

    //load image  in gray level
    input_image = imread(filename,1);

    string filename_whithoutExt=getFilename(filename);
    cout << "Working with file: "<< filename_whithoutExt << "\n";
    //Detect posibles plate regions
    DetectRegions detectRegions;    
    detectRegions.setFilename(filename_whithoutExt);

    // Set saveRegions to save the images to ../tmp
    //detectRegions.saveRegions = false;
    detectRegions.saveRegions = bSegmentPlateOnly;
    // Set showSteps to show each step
    detectRegions.showSteps = bShowSteps;
    //detectRegions.showSteps=true;

    vector<Plate> posible_regions= detectRegions.run( input_image );    

    //If only segment the mat then exit
    if(! bSegmentPlateOnly) {
		//SVM for each plate region to get valid car plates
		//Read file storage.
		FileStorage fs;
		fs.open("SVM.xml", FileStorage::READ);
		Mat SVM_TrainingData;
		Mat SVM_Classes;
		fs["TrainingData"] >> SVM_TrainingData;
		fs["classes"] >> SVM_Classes;

		//Set SVM params
		CvSVMParams SVM_params;
		SVM_params.svm_type = CvSVM::C_SVC;
		SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;
		SVM_params.degree = 0;
		SVM_params.gamma = 1;
		SVM_params.coef0 = 0;
		SVM_params.C = 1;
		SVM_params.nu = 0;
		SVM_params.p = 0;
		SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);
		//Train SVM
		CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);

		//For each possible plate, classify if it's a plate or not with SVM
		vector<Plate> plates;
		for(int i=0; i< posible_regions.size(); i++)
		{
			Mat img=posible_regions[i].plateImg;
			Mat p= img.reshape(1, 1);
			p.convertTo(p, CV_32FC1);

			int response = (int)svmClassifier.predict( p );
			if(response==1)
				plates.push_back(posible_regions[i]);
		}
		cout << "Number of plates detected: " << plates.size() << "\n";

		//For each plate detected, recognize it with OCR
		OCR ocr("OCR.xml");
		//Save each char image to ../tmpChar
		ocr.saveSegments = bSegmentCharOnly;
		//If show each steps
//		ocr.showSteps = false;
		ocr.showSteps = bShowSteps;
		ocr.filename = filename_whithoutExt;
		for(int i=0; i< plates.size(); i++){
			Plate plate=plates[i];

			string plateNumber=ocr.run(&plate);

			if(! bSegmentCharOnly) {
				string licensePlate=plate.str();
				cout << "================================================\n";
				cout << "License plate number: "<< licensePlate << "\n";
				cout << "================================================\n";
				//Show the result
				rectangle(input_image, plate.position, Scalar(0,0,200));
				putText(input_image, licensePlate, Point(plate.position.x, plate.position.y), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,200),2);
				if(bShowSteps){
					imshow("Plate Detected seg", plate.plateImg);
					cvWaitKey(0);
				}
			}
		}

		// To show the final result
		if(! bSegmentCharOnly) {
			imshow("Plate Detected", input_image);
		}
    }

    if(bShowSteps) {
        // Waiting for key to be pressed
        for(;;)
    	{
    	   int c;
    	   c = cvWaitKey(10);

    	   // If press the ESC key then exit
    	   if( (char) c == 27)
    		   break;
    	}
    }

    return 0;

}
