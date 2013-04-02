/*
 * DetectPeople.h
 *
 *  Created on: 2013-4-2
 *      Author: jockeyyan
 */

#ifndef DETECTPEOPLE_H_
#define DETECTPEOPLE_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

Mat detectPeople(Mat img);

#endif /* DETECTPEOPLE_H_ */
