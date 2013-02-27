/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
******************************************************************************
*   Training OCR
*****************************************************************************/

// Main entry code OpenCV

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

#include "OCR.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

//TODO: Number of files of each char
//const int numOfCharFiles[]={35, 40, 42, 41, 42, 33, 30, 31, 49, 44, 30, 24, 21, 20, 34, 9, 10, 3, 11, 3, 15, 4, 9, 12, 10, 21, 18, 8, 15, 7};
//TODO: It should be aligned to OCR::strChars[]
const int numOfCharFiles[] = {
//		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		15, 5, 13, 8, 8, 10, 5, 7, 16, 9,
//		"A", "B", "C", "D", "E", "F", "G",
		13, 3, 0, 3, 5, 3, 0,
//		"H", "J", "K", "L", "M", "N",
		0, 0, 2, 2, 0, 6,
//		"P", "Q", "R", "S", "T",
		2, 0, 0, 4, 0,
//		"U", "V", "W", "X", "Y", "Z",
		1, 0, 0, 1, 0, 0,
//		"京", "津", "冀", "晋", "蒙", "辽", "吉", "黑",
		5, 0, 1, 1, 0, 0, 0, 1,
//		"沪", "苏", "浙", "皖", "闽", "赣", "鲁", "豫",
		2, 1, 2, 0, 0, 0, 2, 0,
//		"鄂", "湘", "粤", "桂", "琼", "渝", "川", "贵", "云", "藏",
		0, 1, 2, 1, 0, 0, 1, 0, 0, 0,
//		"陕", "甘", "青", "宁", "新"
		0, 0, 0, 0, 0
	};

int main1 ( int argc, char** argv )
{
    cout << "Training OCR Automatic Number Plate Recognition\n";

    char* path;
    
    //Check if user specify image to process
    if(argc >= 2 ) {
        path= argv[1];
    } else {
        cout << "Usage:\t" << argv[0] << " <autoplatechars> \n";
        cout << "\tautoplatechars: path contained directories for each char files\n\n";
        return 0;
    }        

    Mat classes;
    Mat trainingDataf5;
    Mat trainingDataf10;
    Mat trainingDataf15;
    Mat trainingDataf20;

    vector<int> trainingLabels;
    OCR ocr;

    for(int i = 0; i < OCR::numOfChars; i++)
    {
        int numFiles = numOfCharFiles[i];
        for(int j = 0; j < numFiles; j++){
            cout << "Processing Char("<< OCR::strChars[i] << ") file name: " << j << ".jpg\n";
            stringstream ss(stringstream::in | stringstream::out);
            ss << path << "/" << OCR::strChars[i] << "/" << j << ".jpg";
            Mat img=imread(ss.str(), 0);
            Mat f5=ocr.features(img, 5);
            Mat f10=ocr.features(img, 10);
            Mat f15=ocr.features(img, 15);
            Mat f20=ocr.features(img, 20);

            trainingDataf5.push_back(f5);
            trainingDataf10.push_back(f10);
            trainingDataf15.push_back(f15);
            trainingDataf20.push_back(f20);
            trainingLabels.push_back(i);
        }
    }

    
    trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
    trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
    trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
    trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
    Mat(trainingLabels).copyTo(classes);

    FileStorage fs("OCR.xml", FileStorage::WRITE);
    fs << "TrainingDataF5" << trainingDataf5;
    fs << "TrainingDataF10" << trainingDataf10;
    fs << "TrainingDataF15" << trainingDataf15;
    fs << "TrainingDataF20" << trainingDataf20;
    fs << "classes" << classes;
    fs.release();

    return 0;
}
