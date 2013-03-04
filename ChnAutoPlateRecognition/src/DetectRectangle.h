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

vector<Plate> segmentInRectangle (Mat imgMat);

#endif
