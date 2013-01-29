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
        vector<Plate> segment(Mat input);
        bool verifySizes(RotatedRect mr);
        Mat histeq(Mat in);
};

#endif
