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
#include "CvxText.h"

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
    bool bDebug = false;

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

    	if((strchr(strCmdOpt, '-')) && (strchr(strCmdOpt, 'd')))
    		bDebug = true;
    } else {
        printf("Usage:\t%s [-s][c][t] image_file\n", argv[0]);
        printf("\t-s: ONLY Segment the plates from image and save to ../tmp\n");
        printf("\t-c: Segment the char from plate and save to ../tmpChar\n");
        printf("\t-t: Show each processing steps\n\n");
        return 0;
    }        

    //Load image in color level
    Mat imgOriginal = imread(filename, CV_LOAD_IMAGE_COLOR);
    //Preprocess and scale to width < 1000 and height < 1000
    Mat imgScale = scalePreprocess(imgOriginal);

    string filename_whithoutExt=getFilename(filename);
    cout << "Working with file: "<< filename_whithoutExt << "\n";

    //Detect possibles plate regions in method1
    DetectRegions detectRegions;    
    detectRegions.setFilename(filename_whithoutExt);
    // Set saveRegions to save the images to ../tmp
    detectRegions.saveRegions = bSegmentPlateOnly;
    // Set showSteps to show each step
    detectRegions.showSteps = bShowSteps;
    detectRegions.debug = bDebug;
    vector<Plate> possible_regions1 = detectRegions.segmentInVertLine(imgScale);

    //Detect possibles plate regions in method2
    DetectRectangle detectRectangle;
    detectRectangle.setFilename(filename_whithoutExt);
    // Set saveRegions to save the images to ../tmp
    detectRectangle.saveRegions = bSegmentPlateOnly;
    // Set showSteps to show each step
    detectRectangle.showSteps = bShowSteps;
    detectRectangle.debug = bDebug;
    vector<Plate> possible_regions2 = detectRectangle.segmentInRectangle(imgScale);

    //Combile two vector of plates
    possible_regions1.insert(possible_regions1.end(), possible_regions2.begin(), possible_regions2.end());

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
		for(int i=0; i< possible_regions1.size(); i++)
		{
			Mat img=possible_regions1[i].plateImg;
			Mat p= img.reshape(1, 1);
			p.convertTo(p, CV_32FC1);

			int response = (int)svmClassifier.predict( p );
			if(response==1)
				plates.push_back(possible_regions1[i]);
		}
		cout << "Number of plates detected: " << plates.size() << "\n";

		//For each plate detected, recognize it with OCR
		OCR ocr("OCR.xml");
		//Save each char image to ../tmpChar
		ocr.saveSegments = bSegmentCharOnly;
		//If show each steps
		ocr.showSteps = bShowSteps;
		ocr.debug = bDebug;
		ocr.filename = filename_whithoutExt;
		for(int i=0; i< plates.size(); i++){
			Plate plate=plates[i];

			string plateNumber = ocr.run(&plate, i);

			if(! bSegmentCharOnly) {
				if(plate.isValidPlate) {
					string licensePlate = plate.getPlateNumStr();
					cout << "================================================\n";
					cout << "License plate #"<< i+1 << " number: "<< licensePlate << "\n";
					cout << "================================================\n";

//					putText(imgScale, licensePlate, Point(plate.position.x, plate.position.y), CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,200),2);
					if(bShowSteps){
//						imshow("Detected Plate", plate.plateImg);
//						cvWaitKey(0);

						//Show the result in image
						rectangle(imgScale, plate.position, Scalar(0,0,200));
						IplImage img = IplImage(imgScale);

						// "wqy-microhei.ttc"为文泉驿黑体
						CvxText text("/usr/share/fonts/truetype/wqy/wqy-microhei.ttc");
						const char *msg = licensePlate.c_str();
						float p = 0.8;
						text.setFont(NULL, NULL, NULL, &p);   // 透明处理
						text.putText(&img, msg, cvPoint(plate.position.x, plate.position.y - 5), CV_RGB(255,0,0));

					   // 定义窗口，并显示影象
					   cvNamedWindow( "Detected_Plate", CV_WINDOW_AUTOSIZE );    //创建用于显示源图像的窗口
					   cvShowImage( "Detected_Plate", &img );        //显示源图像
					   cvWaitKey(-1);                    //等待用户响应
					   cvDestroyWindow( "Detected_Plate" );        //销毁窗口“img”
					}
				} else {
					if(bDebug) {
						string licensePlate = plate.getPlateNumStr();
						cout << "================================================\n";
						cout << "License plate #"<< i+1 << " number: "<< licensePlate << "\n";
						cout << "================================================\n";
					}
				}
			}
		}

		// To show the final result
		if(! bSegmentCharOnly) {
			string wndName = "PlateDetected";
			namedWindow(wndName, WINDOW_AUTOSIZE);
			imshow(wndName, imgScale);
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
