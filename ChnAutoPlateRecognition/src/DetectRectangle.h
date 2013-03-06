/*****************************************************************************
*
*****************************************************************************/

#ifndef DetectRectangle_h
#define DetectRectangle_h

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>

#include "Plate.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

using namespace std;
using namespace cv;

class DetectRectangle {
    public:
		DetectRectangle();
        string filename;
        bool saveRegions;
        bool showSteps;
        bool debug;
        void setFilename(string f);
        vector<Plate> segmentInRectangle (Mat imgMat);
    private:
        int gThresh;
        IplImage* gImg;
        IplImage* gImg0;
        CvMemStorage* gStorage;
        CvPoint gPnt[4];
        const char* wndname;
        double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0);
        CvSeq* findSquares4(IplImage* imgSrc, CvMemStorage* storage);
        //Copy from DetectRegions
        bool verifySizes(RotatedRect mr);
        Mat cropRectOfPlate(Mat imgInput, RotatedRect rectInput);
        Mat histEqualize(Mat in);
};

#endif
