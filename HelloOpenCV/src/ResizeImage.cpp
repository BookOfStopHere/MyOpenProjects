/*
 * ResizeImage.cpp
 *
 *  Created on: 2013-3-6
 *      Author: jockeyyan
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

using namespace cv;
using namespace std;

int IMG_MAX_WIDTH = 500;
int IMG_MAX_HEIGHT = 500;

int main8 (int argc, char** argv)
{
    IplImage *pIplSrc = NULL;
    IplImage *pIplDst = NULL;
    CvSize dstSize;
    double scaleFactor;

    // Input is Mat
    Mat imgSrc = imread("building.jpg", CV_LOAD_IMAGE_COLOR);
    imshow("imgSrc", imgSrc);

    IplImage iplSrc = IplImage(imgSrc);
    pIplSrc = &iplSrc;
//    pIplSrc = cvLoadImage("building.jpg");

    if (pIplSrc == NULL)
    {
        cout << "No Image!" << endl;
        return 0;
    }

    // Calculate the scale factor
    int imgActMax = (pIplSrc->width >= pIplSrc->height) ? pIplSrc->width: pIplSrc->height;
    int imgLower = (IMG_MAX_WIDTH < IMG_MAX_HEIGHT) ? IMG_MAX_WIDTH: IMG_MAX_HEIGHT;
    if(imgActMax > imgLower) {
    	scaleFactor = (double)imgLower / (double)imgActMax;
    } else {
    	scaleFactor = 1.0;
    }
//    cout << "scaleFactor=" << scaleFactor << endl;

    // Scale the image
    dstSize.width = pIplSrc->width * scaleFactor;
    dstSize.height = pIplSrc->height * scaleFactor;
//    cout << "dstSize=" << dstSize.width << "," << dstSize.height << endl;

    pIplDst = cvCreateImage(dstSize, pIplSrc->depth, pIplSrc->nChannels);    //构造目标图象
    cvResize(pIplSrc, pIplDst, CV_INTER_LINEAR);                         //缩放源图像到目标图像

    // Output is Mat and copy data
    Mat imgDst = Mat(pIplDst, TRUE);
    imshow("imgDst", imgDst);

//    // Show the result and save to file
//    cvNamedWindow( "pIplSrc",   CV_WINDOW_AUTOSIZE );    //创建用于显示源图像的窗口
//    cvNamedWindow( "pIplDst",   CV_WINDOW_AUTOSIZE );    //创建用于显示目标图像的窗口
//    cvShowImage( "pIplSrc", pIplSrc );        //显示源图像
//    cvShowImage( "pIplDst", pIplDst );        //显示目标图像
//    cvSaveImage("ImgDst.jpg", pIplDst);      //保存本地
//
//
//    cvDestroyWindow( "pIplSrc" );        //销毁窗口“pIplSrc”
//    cvDestroyWindow( "pIplDst" );        //销毁窗口“pIplDst”

    // Release the image
//    cvReleaseImage(&pIplSrc);            //释放源图像占用的内存

	cvWaitKey(-1);                    //等待用户响应

	cvReleaseImage(&pIplDst);            //释放目标图像占用的内存

    return 1;
}



