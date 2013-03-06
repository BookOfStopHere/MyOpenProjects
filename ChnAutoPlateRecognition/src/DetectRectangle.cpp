/*
 * DetectRectangle.cpp
 *
 *  Created on: 2013-2-28
 *      Author: jockeyyan
 */

#include "DetectRectangle.h"

// Constructor
DetectRectangle::DetectRectangle(){
    showSteps = false;
    saveRegions = false;

    gThresh = 50;
    gImg = 0;
    gImg0 = 0;
    gStorage = 0;
    wndname = "SquareContours";
}

// Set working file name
void DetectRectangle::setFilename(string s) {
    filename = s;
}

// To verify the size of auto plates
bool DetectRectangle::verifySizes(RotatedRect mr){
    float error = 0.4;

    //TODO: Car plate aspect
//    float aspect=4.7272;	//for Spainish car plate size: 52x11 aspect 4,7272
    float aspect = 3.14286;	//for Chinese car plate size: 440*140 aspect 3.14286

    //Set a min and max area. All other patchs are discarded
    int min= 15*aspect*15; // minimum area
    int max= 125*aspect*125; // maximum area

    //Get only patchs that match to a respect ratio.
    float rmin= aspect-aspect*error;
    float rmax= aspect+aspect*error;

    int area = mr.size.height * mr.size.width;
    float r= (float)mr.size.width / (float)mr.size.height;
    if(r<1)
        r= (float)mr.size.height / (float)mr.size.width;

    if(( area < min || area > max ) || ( r < rmin || r > rmax )){
        return false;
    }else{
        return true;
    }

}

// Equalize the Mat
Mat DetectRectangle::histEqualize(Mat in)
{
    Mat out(in.size(), in.type());
    if(in.channels()==3){
        Mat hsv;
        vector<Mat> hsvSplit;
        cvtColor(in, hsv, CV_BGR2HSV);
        split(hsv, hsvSplit);
        equalizeHist(hsvSplit[2], hsvSplit[2]);
        merge(hsvSplit, hsv);
        cvtColor(hsv, out, CV_HSV2BGR);
    }else if(in.channels()==1){
        equalizeHist(in, out);
    }

    return out;

}

// Crop the possible plate and convert to gray image
Mat DetectRectangle::cropRectOfPlate(Mat imgInput, RotatedRect rectInput) {
	Mat grayResult;

    // To verify the size of plate
    if(verifySizes(rectInput)){
        //Rotate the matrix if width < height
        float r = (float)rectInput.size.width / (float)rectInput.size.height;
        float angle = rectInput.angle;
        if(r < 1)
            angle = 90 + angle;
        Mat rotmat = getRotationMatrix2D(rectInput.center, angle, 1);

        //Create and rotate image
        Mat img_rotated;
        warpAffine(imgInput, img_rotated, rotmat, imgInput.size(), CV_INTER_CUBIC);

        //Crop image
        Size rect_size = rectInput.size;
        if(r < 1)
            swap(rect_size.width, rect_size.height);
        Mat img_crop;
        getRectSubPix(img_rotated, rect_size, rectInput.center, img_crop);

    	Mat resultResized;
        // TODO: create the image with the size of Chinese Auto Plate
//            resultResized.create(33, 144, CV_8UC3);	//for Spain
        resultResized.create(35, 110, CV_8UC3);	//for China
        resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);

    	//Equalize croped image
        cvtColor(resultResized, grayResult, CV_BGR2GRAY);
        blur(grayResult, grayResult, Size(3,3));
        grayResult = histEqualize(grayResult);
    }

    return grayResult;
}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double DetectRectangle::angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	return (dx1 * dx2 + dy1 * dy2)
			/ sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
CvSeq* DetectRectangle::findSquares4(IplImage* imgSrc, CvMemStorage* storage)
{
	CvSeq* contours;
	int i, c, l, N = 11;
	CvSize sz = cvSize(imgSrc->width & -2, imgSrc->height & -2);

	IplImage* imgTmp = cvCloneImage(imgSrc); // make a copy of input image
	IplImage* imgGray = cvCreateImage(sz, 8, 1);
	IplImage* imgPyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), 8, 3);
	IplImage* imgGrayTmp;
	CvSeq* result;
	double s, t;

	// create empty sequence that will contain points -
	// 4 points per square (the square's vertices)
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	// select the maximum ROI in the image
	// with the width and height divisible by 2
	cvSetImageROI(imgTmp, cvRect(0, 0, sz.width, sz.height));

	// down-scale and up-scale the image to filter out the noise
	cvPyrDown(imgTmp, imgPyr, 7);
	cvPyrUp(imgPyr, imgTmp, 7);
	imgGrayTmp = cvCreateImage(sz, 8, 1);

	// find squares in every color plane of the image
	for (c = 0; c < 3; c++)
	{
		// extract the c-th color plane
		cvSetImageCOI(imgTmp, c + 1);
		cvCopy(imgTmp, imgGrayTmp, 0);

		// try several threshold levels
		for (l = 0; l < N; l++)
		{
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				cvCanny(imgGrayTmp, imgGray, 0, gThresh, 5);

				// dilate canny output to remove potential
				// holes between edge segments
				cvDilate(imgGray, imgGray, 0, 1);
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				cvThreshold(imgGrayTmp, imgGray, (l + 1) * 255 / N, 255, CV_THRESH_BINARY);
			}

			// find contours and store them all as a list
			cvFindContours(imgGray, storage, &contours, sizeof(CvContour),
					CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

			// test each contour
			while (contours)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				result = cvApproxPoly(contours, sizeof(CvContour), storage,
						CV_POLY_APPROX_DP, cvContourPerimeter(contours) * 0.02, 0);

				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (result->total == 4 &&
						fabs(cvContourArea(result, CV_WHOLE_SEQ)) > 1000 &&
						cvCheckContourConvexity(result))
				{
					s = 0;

					for (i = 0; i < 5; i++)
					{
						// find minimum angle between joint edges (maximum of cosine)
						if (i >= 2)
						{
							t = fabs(angle(
									(CvPoint*) cvGetSeqElem(result, i),
									(CvPoint*) cvGetSeqElem(result, i - 2),
									(CvPoint*) cvGetSeqElem(result, i - 1)));
							s = s > t ? s : t;
						}
					}

					// if cosines of all angles are small
					// (all angles are ~90 degree) then write quad range
					// vertices to resultant sequence
					if (s < 0.3)
						for (i = 0; i < 4; i++)
							cvSeqPush(squares, (CvPoint*) cvGetSeqElem(result, i));
				}

				// take the next contour
				contours = contours->h_next;
			}
		}
	}

	// release all the temporary images
	cvReleaseImage(&imgGray);
	cvReleaseImage(&imgPyr);
	cvReleaseImage(&imgGrayTmp);
	cvReleaseImage(&imgTmp);

	return squares;
}

//// the function draws all the squares in the image
//void drawSquares(IplImage* imgSrc, CvSeq* squares)
//{
//	CvSeqReader reader;
//	IplImage* imgCopy = cvCloneImage(imgSrc);
//	int i;
//
//	// initialize reader of the sequence
//	cvStartReadSeq(squares, &reader, 0);
//
//	// read 4 sequence elements at a time (all vertices of a square)
//	printf("Found %d rectangles in image\n", squares->total / 4);
//
//	for (i = 0; i < squares->total; i += 4)
//	{
//		CvPoint* pntRect = gPnt;
//		int pntCount = 4;
//		CvSeq* seqRect = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), gStorage);
//
//		// read 4 vertices
//		memcpy(gPnt, reader.ptr, squares->elem_size);
//		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
//		cvSeqPush(seqRect, &pntRect[0]);
//
//		memcpy(gPnt + 1, reader.ptr, squares->elem_size);
//		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
//		cvSeqPush(seqRect, &pntRect[1]);
//
//		memcpy(gPnt + 2, reader.ptr, squares->elem_size);
//		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
//		cvSeqPush(seqRect, &pntRect[2]);
//
//		memcpy(gPnt + 3, reader.ptr, squares->elem_size);
//		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
//		cvSeqPush(seqRect, &pntRect[3]);
//
//		// draw the square as a closed polyline
//		cvPolyLine(imgCopy, &pntRect, &pntCount, 1, 1, CV_RGB(0, 255, 0), 1, CV_AA, 0);
//
//		// convert to rotatedRect
//		CvBox2D box = cvMinAreaRect2(seqRect, NULL);
//		RotatedRect rectRot = RotatedRect(box);
//
//		// draw the min outter rect
//	    CvPoint2D32f ptBox[4];
//	    cvBoxPoints(box, ptBox);
//	    for(int i = 0; i < 4; ++i) {
//	        cvLine(imgCopy, cvPointFrom32f(ptBox[i]), cvPointFrom32f(ptBox[((i+1)%4)?(i+1):0]), CV_RGB(255,0,0));
//	    }
//
//	}
//
//	// show the resultant image
//	cvShowImage(wndname, imgCopy);
//	cvReleaseImage(&imgCopy);
//}

//Segment plate in rectangle
vector<Plate> DetectRectangle::segmentInRectangle (Mat imgMat)
{
    vector<Plate> output;
    IplImage imgSrc = IplImage(imgMat);

	// create memory storage that will contain all the dynamic data
	gStorage = cvCreateMemStorage(0);
	gImg = cvCloneImage(&imgSrc);

	cvNamedWindow(wndname, CV_WINDOW_NORMAL);

	if (gImg) {
		CvSeq* squares = findSquares4(gImg, gStorage);

		// for each rectangle, get rectangle pos, crop and convert to plate
		CvSeqReader reader;
		IplImage* imgCopy = cvCloneImage(&imgSrc);
		int i;

		// initialize reader of the sequence
		cvStartReadSeq(squares, &reader, 0);

		// read 4 sequence elements at a time (all vertices of a square)
		if(debug) {
			printf("DetectRectangle found %d rectangles in image %s\n", squares->total / 4, filename.c_str());
		}

		for (i = 0; i < squares->total; i += 4)
		{
			CvPoint* pntRect = gPnt;
			int pntCount = 4;
			CvSeq* seqRect = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), gStorage);

			// read 4 vertices
			memcpy(gPnt, reader.ptr, squares->elem_size);
			CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
			cvSeqPush(seqRect, &pntRect[0]);

			memcpy(gPnt + 1, reader.ptr, squares->elem_size);
			CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
			cvSeqPush(seqRect, &pntRect[1]);

			memcpy(gPnt + 2, reader.ptr, squares->elem_size);
			CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
			cvSeqPush(seqRect, &pntRect[2]);

			memcpy(gPnt + 3, reader.ptr, squares->elem_size);
			CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
			cvSeqPush(seqRect, &pntRect[3]);

			// draw the square as a closed polyline with GREEN
			cvPolyLine(imgCopy, &pntRect, &pntCount, 1, 1, CV_RGB(0, 255, 0), 1, CV_AA, 0);

			// convert to rotatedRect and save to output
			CvBox2D box = cvMinAreaRect2(seqRect, NULL);
			RotatedRect rectPlate = RotatedRect(box);

			if(debug) {
				printf("Rect #%d center[x=%f, y=%f]\n", i/4, rectPlate.center.x, rectPlate.center.y);
			}

			Mat imgGray = cropRectOfPlate(imgMat, rectPlate);
			if(imgGray.total() != 0) {
				//
				if(showSteps) {
					imshow(wndname, imgGray);
					cvWaitKey(0);
				}
				// Add to result
				output.push_back(Plate(imgGray, rectPlate.boundingRect()));

	            // Save the blured gray image to file
	            if(saveRegions) {
	                stringstream ss(stringstream::in | stringstream::out);
	                ss << "../tmp/" << filename << "_r_" << i << ".jpg";
	                imwrite(ss.str(), imgGray);
	            }

				// draw the rotated rectangle with RED
			    CvPoint2D32f ptBox[4];
			    cvBoxPoints(box, ptBox);
			    for(int i = 0; i < 4; ++i) {
			        cvLine(imgCopy, cvPointFrom32f(ptBox[i]), cvPointFrom32f(ptBox[((i+1)%4)?(i+1):0]), CV_RGB(255,0,0));
			    }
			}
		}

		// show the resultant image
		if(showSteps) {
			cvShowImage(wndname, imgCopy);
			cvWaitKey(0);
		}

		cvReleaseImage(&imgCopy);
	}

	// release both images
	cvReleaseImage(&gImg);
	cvReleaseImage(&gImg0);

	// clear memory storage - reset free space position
	cvClearMemStorage(gStorage);

//    for(;;)
//	{
//    	// wait for key.
//    	// Also the function cvWaitKey takes care of event processing
//    	int c = cvWaitKey(0);
//
//    	if (c == 27)
//    		break;
//	}

	cvDestroyWindow(wndname);

    return output;
}
