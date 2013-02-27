/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
******************************************************************************
*
*****************************************************************************/

#ifndef Plate_h
#define Plate_h

#include <string.h>
#include <string>
#include <vector>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

using namespace std;
using namespace cv;

class Plate{
    public:
        Plate();
        Plate(Mat img, Rect pos);
        string str();
        Rect position;
        Mat plateImg;
//        vector<char> chars;	//for char not contained HZ
        vector<string> chars;	//for char contained HZ
        vector<Rect> charsPos;
};

#endif
