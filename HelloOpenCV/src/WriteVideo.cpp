#include <iostream>	// for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

#include "DetectPeople.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Not enough parameters" << endl;
        return -1;
    }

    const string srcFile = argv[1];            // the source file name

    VideoCapture inputVideo(srcFile);        // Open input
    if ( !inputVideo.isOpened())
    {
        cout  << "Could not open the input video." << srcFile << endl;
        return -1;
    }

    const string outFile = srcFile + ".out.avi";   // Form the new name with container
    int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form

//    // Transform from int to char via Bitwise operators
//    char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0};

    // Acquire input size
    Size frameSize = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
                  	  	  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    // Open the output
    VideoWriter outputVideo;
//	outputVideo.open(outFile, ex=-1, inputVideo.get(CV_CAP_PROP_FPS), frameSize, true);
    outputVideo.open(outFile, ex, inputVideo.get(CV_CAP_PROP_FPS), frameSize, true);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << outFile << endl;
        return -1;
    }

//    union { int v; char c[5];} uEx ;
//    uEx.v = ex;                              // From Int to char via union
//    uEx.c[4]='\0';

    cout << "Input frame resolution: Width=" << frameSize.width << ", Height=" << frameSize.height
    		<< ", Number of frames: " << inputVideo.get(CV_CAP_PROP_FRAME_COUNT) << endl;
//    cout << "Input codec type: " << EXT << endl;

//    int channel = 2;    // Select the channel to save
//    switch(argv[2][0])
//    {
//    case 'R' : {channel = 2; break;}
//    case 'G' : {channel = 1; break;}
//    case 'B' : {channel = 0; break;}
//    }

    Mat src,res;
//    vector<Mat> spl;
	long currentFrame = 0;

    while(true) //Show the image captured in the window and repeat
    {
    	// Read
        inputVideo >> src;
        // Check if at end
        if( src.empty())
        	break;

        cout << "Processing the #" << currentFrame << " frame..." << endl;
        Mat res = detectPeople(src);

//       split(src, spl);                 // process - extract only the correct channel
//       for( int i =0; i < 3; ++i)
//        if (i != channel)
//           spl[i] = Mat::zeros(S, spl[0].type());
//       merge(spl, res);

       //outputVideo.write(res); //save or
       outputVideo << res;
//        outputVideo << src;

       currentFrame++;
    }

    cout << "Finished writing to file " << outFile << endl;
    return 0;
}
