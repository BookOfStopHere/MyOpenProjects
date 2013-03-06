/*
 * ResizeImage.cpp
 *
 *  Created on: 2013-3-6
 *      Author: jockeyyan
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    IplImage *src = NULL;
    IplImage *dst = NULL;
    CvSize dstSize;
    double times = 0.5;
    src = cvLoadImage("building.jpg");
    if (src == NULL)
    {
        cout << "No Image!" << endl;
        return 0;
    }
    dstSize.width = src->width * times;        //目标图像的宽为源图象宽的times倍
    dstSize.height = src->height * times;      //目标图像的高为源图象高的times倍

    dst = cvCreateImage(dstSize, src->depth, src->nChannels);    //构造目标图象
    cvResize(src, dst, CV_INTER_LINEAR);                         //缩放源图像到目标图像

    cvNamedWindow( "src",   CV_WINDOW_AUTOSIZE );    //创建用于显示源图像的窗口
    cvNamedWindow( "dst",   CV_WINDOW_AUTOSIZE );    //创建用于显示目标图像的窗口

    cvShowImage( "src", src );        //显示源图像
    cvShowImage( "dst", dst );        //显示目标图像
    cvSaveImage("dst.jpg", dst);      //保存本地

    cvWaitKey(-1);                    //等待用户响应

    cvReleaseImage(&src);            //释放源图像占用的内存
    cvReleaseImage(&dst);            //释放目标图像占用的内存
    cvDestroyWindow( "src" );        //销毁窗口“src”
    cvDestroyWindow( "dst" );        //销毁窗口“dst”
    return 1;
}



