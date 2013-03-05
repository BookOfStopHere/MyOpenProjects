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
        bool saveRegions;
        bool showSteps;
        void setFilename(string f);
        vector<Plate> segmentInVertLine(Mat imgInput);
    private:
        vector<RotatedRect> detectRectInVertline(Mat imgInput);
        bool verifySizes(RotatedRect mr);
        Mat cropRectOfPlate(Mat imgInput, RotatedRect rectInput);
        Mat histEqualize(Mat in);
};

#endif

