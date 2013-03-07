/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
******************************************************************************
*
*****************************************************************************/

#include "Plate.h"

Plate::Plate(){
}

Plate::Plate(Mat img, Rect pos){
    plateImg=img;
    position=pos;
}

string Plate::str(){
    string result="";

    //Order numbers
    vector<int> orderIndex;
    vector<int> xpositions;
    for(int i=0; i< charsPos.size(); i++){
        orderIndex.push_back(i);
        xpositions.push_back(charsPos[i].x);
    }
    float min=xpositions[0];
    int minIdx=0;
    for(int i=0; i< xpositions.size(); i++){
        min=xpositions[i];
        minIdx=i;
        for(int j=i; j<xpositions.size(); j++){
            if(xpositions[j]<min){
                min=xpositions[j];
                minIdx=j;
            }
        }
        int aux_i=orderIndex[i];
        int aux_min=orderIndex[minIdx];
        orderIndex[i]=aux_min;
        orderIndex[minIdx]=aux_i;
        
        float aux_xi=xpositions[i];
        float aux_xmin=xpositions[minIdx];
        xpositions[i]=aux_xmin;
        xpositions[minIdx]=aux_xi;
    }
    for(int i=0; i<orderIndex.size(); i++){
        result=result+chars[orderIndex[i]];
    }
    return result;
}

// GLOBAL HELPER FUNCTION
// Preprocess image, scale to specified size
Mat scalePreprocess(Mat imgSrc)
{
    IplImage *pIplSrc = NULL;
    IplImage *pIplDst = NULL;
    CvSize dstSize;
    double scaleFactor;
    Mat imgDst;

    // Input is Mat
//    Mat imgSrc = imread("building.jpg", CV_LOAD_IMAGE_COLOR);

    IplImage iplSrc = IplImage(imgSrc);
    pIplSrc = &iplSrc;
//    pIplSrc = cvLoadImage("building.jpg");

    if (pIplSrc == NULL)
    {
        cout << "No Image!" << endl;
        return imgDst;
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
    imgDst = Mat(pIplDst, TRUE);

//    // Show image
//    imshow("imgSrc", imgSrc);
//    imshow("imgDst", imgDst);
//	cvWaitKey(-1);                    //等待用户响应

	cvReleaseImage(&pIplDst);            //释放目标图像占用的内存

    return imgDst;
}

