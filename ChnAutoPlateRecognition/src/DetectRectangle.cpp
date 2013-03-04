/*
 * DetectRectangle.cpp
 *
 *  Created on: 2013-2-28
 *      Author: jockeyyan
 */

#include "DetectRectangle.h"

int gThresh = 50;
IplImage* gImg = 0;
IplImage* gImg0 = 0;
CvMemStorage* gStorage = 0;
CvPoint gPnt[4];

const char* wndname = "SquareContours";

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
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
CvSeq* findSquares4(IplImage* imgSrc, CvMemStorage* storage)
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

// the function draws all the squares in the image
void drawSquares(IplImage* imgSrc, CvSeq* squares)
{
	CvSeqReader reader;
	IplImage* imgCopy = cvCloneImage(imgSrc);
	int i;

	// initialize reader of the sequence
	cvStartReadSeq(squares, &reader, 0);

	// read 4 sequence elements at a time (all vertices of a square)
	printf("Found %d rectangles in image\n", squares->total / 4);

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

		// draw the square as a closed polyline
		cvPolyLine(imgCopy, &pntRect, &pntCount, 1, 1, CV_RGB(0, 255, 0), 1, CV_AA, 0);

		// convert to rotatedRect
		CvBox2D box = cvMinAreaRect2(seqRect, NULL);
		RotatedRect rectRot = RotatedRect(box);

		// draw the min outter rect
	    CvPoint2D32f ptBox[4];
	    cvBoxPoints(box, ptBox);
	    for(int i = 0; i < 4; ++i) {
	        cvLine(imgCopy, cvPointFrom32f(ptBox[i]), cvPointFrom32f(ptBox[((i+1)%4)?(i+1):0]), CV_RGB(255,0,0));
	    }

	}

	// show the resultant image
	cvShowImage(wndname, imgCopy);
	cvReleaseImage(&imgCopy);
}

//Segment plate in rectangle
vector<Plate> segmentInRectangle (Mat imgMat)
{
    vector<Plate> output;
    IplImage imgSrc = IplImage(imgMat);

	// create memory storage that will contain all the dynamic data
	gStorage = cvCreateMemStorage(0);
	gImg = cvCloneImage(&imgSrc);

	cvNamedWindow(wndname, 1);

	if (gImg) {
		CvSeq* squares = findSquares4(gImg, gStorage);

		// for each rectangle, get rectangle pos, crop and convert to plate
		drawSquares(gImg, squares);

//        Mat imgGray = cropRectOfPlate(imgInput, rectPlate);
//    	output.push_back(Plate(imgGray, rectPlate.boundingRect()));
	}

	// release both images
	cvReleaseImage(&gImg);
	cvReleaseImage(&gImg0);

	// clear memory storage - reset free space position
	cvClearMemStorage(gStorage);

    for(;;)
	{
    	// wait for key.
    	// Also the function cvWaitKey takes care of event processing
    	int c = cvWaitKey(0);

    	if (c == 27)
    		break;
	}

	cvDestroyWindow(wndname);

    return output;
}
