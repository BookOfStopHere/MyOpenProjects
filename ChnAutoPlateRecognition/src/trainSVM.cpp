/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
******************************************************************************
*   Trainging SVM and save to SVM.xml
*****************************************************************************/

// Main entry code OpenCV

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int main2 ( int argc, char** argv )
{
    cout << "Training SVM for Automatic Number Plate Recognition\n";
    // Command as following:
    // 		./trainSVM 30 30 ../autoplates/ ../nonautoplates/

    char* path_Plates;
    char* path_NoPlates;
    int numPlates;
    int numNoPlates;

//	Spain Auto Plates
//    int imageWidth=144;
//    int imageHeight=33;

//	TODO Chinese Auto Plates, origin is 440*140 mm
//    int imageWidth=110;
//    int imageHeight=35;

    //Check if user specify images to process
    if(argc >= 5 )
    {
        numPlates = atoi(argv[1]);
        numNoPlates = atoi(argv[2]);
        path_Plates = argv[3];
        path_NoPlates = argv[4];

    }else{
        cout << "Usage:\n\t" << argv[0] << " <num of Plate> <num of Non Plate> <path to plate folder> <path to non plate folder> \n";
        cout << "\n";
        return 0;
    }        

    Mat classes;//(numPlates+numNoPlates, 1, CV_32FC1);
    Mat trainingData;//(numPlates+numNoPlates, imageWidth*imageHeight, CV_32FC1 );

    Mat trainingImages;
    vector<int> trainingLabels;

    for(int i=0; i< numPlates; i++)
    {
        stringstream ss(stringstream::in | stringstream::out);
        ss << path_Plates << i << ".jpg";
        Mat img = imread(ss.str(), 0);
        img = img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(1);
    }

    for(int i=0; i< numNoPlates; i++)
    {
        stringstream ss(stringstream::in | stringstream::out);
        ss << path_NoPlates << i << ".jpg";
        Mat img = imread(ss.str(), 0);
        img = img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(0);

    }

    Mat(trainingImages).copyTo(trainingData);
    //trainingData = trainingData.reshape(1,trainingData.rows);
    trainingData.convertTo(trainingData, CV_32FC1);
    Mat(trainingLabels).copyTo(classes);

    FileStorage fs("SVM.xml", FileStorage::WRITE);
    fs << "TrainingData" << trainingData;
    fs << "classes" << classes;
    fs.release();

    return 0;
}
