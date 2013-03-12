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

#ifndef IMG_MAX_WIDTH
#define IMG_MAX_WIDTH 1000
#endif

#ifndef IMG_MAX_HEIGHT
#define IMG_MAX_HEIGHT 800
#endif

Mat scalePreprocess(Mat imgSrc);

class Plate{
    public:
        Plate();
        Plate(Mat img, Rect pos);
        string getPlateNumStr();
        Rect position;
        Mat plateImg;
    	bool isValidPlate;
        void setPlateStatus(bool status);
//        vector<char> chars;	//for char not contained HZ
        vector<string> chars;	//for char contained HZ
        vector<Rect> charsPos;
};

#endif
