//============================================================================
// Name        : HelloOpenCV.cpp
// Author      : Jockey
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <opencv/highgui.h>

int main1(int argc, char** argv) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

    IplImage* pImg = cvLoadImage(argv[1]);
    cvNamedWindow("Sr", 1);
    cvShowImage("Sr", pImg);
    cvWaitKey(0);
    cvDestroyWindow("Sr");
    cvReleaseImage(&pImg);

	return 0;


}
