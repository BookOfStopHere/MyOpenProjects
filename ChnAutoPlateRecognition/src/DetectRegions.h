/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
******************************************************************************
*
*****************************************************************************/

#ifndef DetectRegions_h
#define DetectRegions_h

#include <stdio.h>
#include <string.h>
#include <vector>

#include "Plate.h"
#include "DetectRectangle.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

using namespace std;
using namespace cv;

class DetectRegions{
    public:
        DetectRegions();
        string filename;
        void setFilename(string f);
        bool saveRegions;
        bool showSteps;
        vector<Plate> run(Mat input);
    private:
        vector<Plate> segmentInVertLine(Mat imgInput);
        vector<RotatedRect> detectRectInVertline(Mat imgInput);
//        vector<Plate> segmentInRectangle(Mat imgInput);
        Mat cropRectOfPlate(Mat imgInput, RotatedRect rectInput);
        bool verifySizes(RotatedRect mr);
        Mat histEqulize(Mat in);
};

#endif

