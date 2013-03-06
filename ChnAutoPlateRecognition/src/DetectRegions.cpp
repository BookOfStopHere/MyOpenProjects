/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
*****************************************************************************/

#include "DetectRegions.h"

// Constructor
DetectRegions::DetectRegions(){
    showSteps = false;
    saveRegions = false;
}

// Set working file name
void DetectRegions::setFilename(string s) {
        filename = s;
}

// To verify the size of auto plates
bool DetectRegions::verifySizes(RotatedRect mr){
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
Mat DetectRegions::histEqualize(Mat in)
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

// Detect rectangle with vertical lines
vector<RotatedRect> DetectRegions::detectRectInVertline(Mat imgInput)
{
    //convert image to gray
    Mat img_gray;
    cvtColor(imgInput, img_gray, CV_BGR2GRAY);
    blur(img_gray, img_gray, Size(5,5));

    //Find vertical lines. Car plates have high density of vertical lines
    Mat img_sobel;
    Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
//    if(showSteps)
//        imshow("Sobel", img_sobel);

    //threshold image
    Mat img_threshold;
    threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
//    if(showSteps)
//        imshow("Threshold", img_threshold);

    //Morphplogic operation close
//    Mat element = getStructuringElement(MORPH_RECT, Size(17, 3) );
    Mat element = getStructuringElement(MORPH_RECT, Size(15, 3) );
    morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);
//    if(showSteps)
//        imshow("Close", img_threshold);

    //Find contours of possibles plates
    vector< vector< Point> > contours;
    findContours(img_threshold,
            contours, // a vector of contours
            CV_RETR_EXTERNAL, // retrieve the external contours
            CV_CHAIN_APPROX_NONE); // all pixels of each contours

    //Start to iterate to each contour found
    vector<vector<Point> >::iterator itc = contours.begin();
    //Return value
    vector<RotatedRect> vecRects;

    //Remove patch that are no inside limits of aspect ratio and area.
    while (itc != contours.end()) {
        //Create bounding rect of object
        RotatedRect mr = minAreaRect(Mat(*itc));
        if( !verifySizes(mr)){
            itc = contours.erase(itc);
        }else{
            ++itc;
            vecRects.push_back(mr);
        }
    }

    //Draw blue contours on a white image
    cv::Mat imgResult;
    imgInput.copyTo(imgResult);
    cv::drawContours(imgResult, contours,
            -1, // draw all contours
            cv::Scalar(255,0,0), // in BLUE
            1); // with a thickness of 1

	if(debug) {
		string wndName = "Contours_in_vert_line";
		namedWindow(wndName, WINDOW_NORMAL);
		imshow(wndName, imgResult);
		cvWaitKey(0);
	}

    return vecRects;
}

// Crop the possible plate and convert to gray image
Mat DetectRegions::cropRectOfPlate(Mat imgInput, RotatedRect rectInput) {
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

// Segment all possible area of auto plates
vector<Plate> DetectRegions::segmentInVertLine(Mat imgInput){
    vector<Plate> output;
	cv::Mat imgResult;
	imgInput.copyTo(imgResult);

    vector<RotatedRect> vecRects;
    vecRects = DetectRegions::detectRectInVertline(imgInput);

    for(int i = 0; i < vecRects.size(); i++) {
        //Draw green minAreaRect with Green
		Point2f vertices[4];
		vecRects[i].points(vertices);

		//TODO: DEBUG Show the index of rects
		if(debug) {
			for (int n = 0; n < 4; n++)
				line(imgResult, vertices[n], vertices[(n+1)%4], Scalar(0,255,0));	//GREEN
			char str1[5];
			sprintf(str1, "%d", i);
			putText(imgResult, str1, vertices[0], CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0), 2);
	        circle(imgResult, vecRects[i].center, 3, Scalar(0,255,0), -1);	//GREEN for center point
		}

    	//For better rect cropping for each possible box, make floodfill algorithm for it
        //get the min and max size between width and height
        float minSize=(vecRects[i].size.width < vecRects[i].size.height)? vecRects[i].size.width: vecRects[i].size.height;
        minSize = minSize - minSize*0.5;

        //Radius and Angle, radian=angle*Pi/180, angle=radian*180/Pi
        float bevelSize1, bevelSize2, bevelSize3, bevelSize4;
        float angleAlpha, angleBeta1, angleTheta1, angleBeta2, angleTheta2, angleBeta3, angleTheta3, angleBeta4, angleTheta4;
        angleAlpha = abs(vecRects[i].angle * 3.1415 / 180);	//in Radian
        if((vecRects[i].size.width > vecRects[i].size.height)) {
        	// 1/8
            bevelSize1 = sqrt(vecRects[i].size.width * vecRects[i].size.width * 1/64 + vecRects[i].size.height * vecRects[i].size.height /4);
            angleBeta1 = abs(atan2((vecRects[i].size.height /2), (vecRects[i].size.width * 1/8)));
            angleTheta1 = angleAlpha + angleBeta1;
            // 3/4
            bevelSize2 = sqrt(vecRects[i].size.width * vecRects[i].size.width * 9/16 + vecRects[i].size.height * vecRects[i].size.height /4);
            angleBeta2 = abs(atan2((vecRects[i].size.height /2), (vecRects[i].size.width * 3/4)));
            angleTheta2 = angleAlpha + angleBeta2;
        	// 3/8
            bevelSize3 = sqrt(vecRects[i].size.width * vecRects[i].size.width * 9/64 + vecRects[i].size.height * vecRects[i].size.height /4);
            angleBeta3 = abs(atan2((vecRects[i].size.height /2), (vecRects[i].size.width * 3/8)));
            angleTheta3 = angleAlpha + angleBeta3;
            // 7/8
            bevelSize4 = sqrt(vecRects[i].size.width * vecRects[i].size.width * 49/64 + vecRects[i].size.height * vecRects[i].size.height /4);
            angleBeta4 = abs(atan2((vecRects[i].size.height /2), (vecRects[i].size.width * 7/8)));
            angleTheta4 = angleAlpha + angleBeta4;
        } else {
        	// 1/8
            bevelSize1 = sqrt(vecRects[i].size.width * vecRects[i].size.width /4 + vecRects[i].size.height * vecRects[i].size.height * 1/64);
            angleBeta1 = abs(atan2((vecRects[i].size.height * 1/8), (vecRects[i].size.width /2)));
            angleTheta1 = angleAlpha + angleBeta1;
            // 3/4
            bevelSize2 = sqrt(vecRects[i].size.width * vecRects[i].size.width /4 + vecRects[i].size.height * vecRects[i].size.height * 9/16);
            angleBeta2 = abs(atan2((vecRects[i].size.height * 3/4), (vecRects[i].size.width /2)));
            angleTheta2 = angleAlpha + angleBeta2;
        	// 3/8
            bevelSize3 = sqrt(vecRects[i].size.width * vecRects[i].size.width /4 + vecRects[i].size.height * vecRects[i].size.height * 9/64);
            angleBeta3 = abs(atan2((vecRects[i].size.height * 3/8), (vecRects[i].size.width /2)));
            angleTheta3 = angleAlpha + angleBeta3;
        	// 7/8
            bevelSize4 = sqrt(vecRects[i].size.width * vecRects[i].size.width /4 + vecRects[i].size.height * vecRects[i].size.height * 49/64);
            angleBeta4 = abs(atan2((vecRects[i].size.height * 7/8), (vecRects[i].size.width /2)));
            angleTheta4 = angleAlpha + angleBeta4;
        }

        Point ptSample1, ptSinCos1;
        ptSinCos1.x = bevelSize1 * sin(angleTheta1);	//sin(angle)
        ptSinCos1.y = bevelSize1 * cos(angleTheta1);	//cos(angle)
		ptSample1.x = vertices[0].x + ptSinCos1.y;
		ptSample1.y = vertices[0].y - ptSinCos1.x;

        Point ptSample2, ptSinCos2;
        ptSinCos2.x = bevelSize2 * sin(angleTheta2);	//sin(angle)
        ptSinCos2.y = bevelSize2 * cos(angleTheta2);	//cos(angle)
		ptSample2.x = vertices[0].x + ptSinCos2.y;
		ptSample2.y = vertices[0].y - ptSinCos2.x;

        Point ptSample3, ptSinCos3;
        ptSinCos3.x = bevelSize3 * sin(angleTheta3);	//sin(angle)
        ptSinCos3.y = bevelSize3 * cos(angleTheta3);	//cos(angle)
		ptSample3.x = vertices[0].x + ptSinCos3.y;
		ptSample3.y = vertices[0].y - ptSinCos3.x;

        Point ptSample4, ptSinCos4;
        ptSinCos4.x = bevelSize4 * sin(angleTheta4);	//sin(angle)
        ptSinCos4.y = bevelSize4 * cos(angleTheta4);	//cos(angle)
		ptSample4.x = vertices[0].x + ptSinCos4.y;
		ptSample4.y = vertices[0].y - ptSinCos4.x;

        //TODO: DEBUG
		if(debug) {
			circle(imgResult, ptSample1, 3, Scalar(0,255,255), -1);	//YELLOW for sample1 point
			circle(imgResult, ptSample2, 3, Scalar(0,255,255), -1);	//YELLOW for sample2 point
			circle(imgResult, ptSample3, 3, Scalar(0,255,255), -1);	//YELLOW for sample3 point
			circle(imgResult, ptSample4, 3, Scalar(0,255,255), -1);	//YELLOW for sample4 point
			cout << "Sample point coordinate: ptSample1=" << ptSample1 << "," << "ptSample2=" << ptSample2 << ","
					<< "ptSample3=" << ptSample3 << "," << "ptSample4=" << ptSample4 << "\n";

//			cout << "GREEN Rectangle[" << i << "] region of possible plate: \n"
//					<< "first point x=" << vertices[0].x << ", y=" << vertices[0].y << "\n"
//					<< "sample1 point x=" << ptSample1.x << ", y=" << ptSample1.y << "\n"
//					<< "bevelSize1=" << bevelSize1 << ", angleAlpha=" << angleAlpha * 180 / 3.1415 << ", angleBeta1=" << angleBeta1  * 180 / 3.1415 << ", angleTheta1=" << angleTheta1  * 180 / 3.1415 << "\n"
//					<< "sin()=" << ptSinCos1.x << ", cos()=" << ptSinCos1.y << "\n"
//					<< "sample2 point x=" << ptSample2.x << ", y=" << ptSample2.y << "\n"
//					<< "width=" << vecRects[i].size.width << ", height=" << vecRects[i].size.height << "\n";
		}

        //Initialize rand and get n points around center
		//for floodfill algorithm
        srand ( time(NULL) );
        //Initialize floodfill parameters and variables
        Mat imgMask;
        imgMask.create(imgInput.rows + 2, imgInput.cols + 2, CV_8UC1);
        imgMask = Scalar::all(0);	//initial it with BLACK
        int loDiff = 50;	//???30
        int upDiff = 50;	//???30
        //Set the mask value and connectivity
        int connectivity = 8;	//4
        int newMaskVal = 255;	//255 new color is WHITE
        int flags = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
//        int flags = connectivity + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
//        int flags2 = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE;
//        int flags2 = connectivity + CV_FLOODFILL_FIXED_RANGE;
        int nSeeds = 28;	//4*7 //10
        Rect rectOut;
//        cv::Mat input2;
        for(int j = 0; j < nSeeds; j++) {
        	//TODO: To set the seed point properly
            Point seed;
//            seed.x = rects[i].center.x + rand()%(int)minSize - (minSize/2);
//            seed.y = rects[i].center.y + rand()%(int)minSize - (minSize/2);
            if(j < (nSeeds / 4)) {
                seed.x = ptSample1.x + rand()%(int)minSize - (minSize/2);
                seed.y = ptSample1.y + rand()%(int)minSize - (minSize/2);
            } else if ((j >= (nSeeds /4)) && (j < (nSeeds /2))) {
                seed.x = ptSample2.x + rand()%(int)minSize - (minSize/2);
                seed.y = ptSample2.y + rand()%(int)minSize - (minSize/2);
            } else if ((j >= (nSeeds /2)) && (j < (nSeeds * 3/4))) {
                seed.x = ptSample3.x + rand()%(int)minSize - (minSize/2);
                seed.y = ptSample3.y + rand()%(int)minSize - (minSize/2);
            } else {
                seed.x = ptSample4.x + rand()%(int)minSize - (minSize/2);
                seed.y = ptSample4.y + rand()%(int)minSize - (minSize/2);
            }

//            //TODO: DEBUG Draw seed point
    		if(debug) {
    			cout << "Seed " << j << " point coordinate=" << seed << "\n";
    			circle(imgResult, seed, 1, Scalar(0,255,255), -1);	//Draw seed point with YELLOW
    		}

    		if((seed.x > 0) && (seed.x < imgInput.cols) && (seed.y > 0) && (seed.y < imgInput.rows)) {
                floodFill(imgInput, imgMask,
                		seed,
                		Scalar(0,255,255),
                		&rectOut,	//newVal=YELLOW is ignored when mask is exist
                		Scalar(loDiff, loDiff, loDiff),
                		Scalar(upDiff, upDiff, upDiff),
                		flags);
    		}

//            //TODO: Show the result of floodfilling
//            input.copyTo(input2);
//            floodFill(input2, seed, Scalar(0,255,255), &rectOut,
//            				Scalar(loDiff, loDiff, loDiff), Scalar(upDiff, upDiff, upDiff), flags2);
        }

//        if(showSteps) {
////            imshow("FloodFillMask", mask);
//        	imshow("FloodFill", input2);
//            cvWaitKey(0);
//        }

        //Check new floodfill mask match for a correct patch.
        //Get all points detected for get Minimal rotated Rect
        vector<Point> pointsInterest;
        Mat_<uchar>::iterator itMask= imgMask.begin<uchar>();
        Mat_<uchar>::iterator end= imgMask.end<uchar>();
        for( ; itMask != end; ++itMask)
            if(*itMask == 255)	//The WHITE point is contours
                pointsInterest.push_back(itMask.pos());

        RotatedRect rectPlate = minAreaRect(pointsInterest);
        if(verifySizes(rectPlate)) {
            Mat imgGray = cropRectOfPlate(imgInput, rectPlate);
        	output.push_back(Plate(imgGray, rectPlate.boundingRect()));

            // Save the blured gray image to file
            if(saveRegions){
                stringstream ss(stringstream::in | stringstream::out);
                ss << "../tmp/" << filename << "_v_" << i << ".jpg";
                imwrite(ss.str(), imgGray);
            }

            //Drawing the rotated rectangle with RED
			Point2f pntRectInput[4];
			rectPlate.points( pntRectInput );
			for( int j = 0; j < 4; j++ ) {
				line( imgResult, pntRectInput[j], pntRectInput[(j+1)%4], Scalar(0,0,255), 1, 8 );
			}
        }
    }       

    if(showSteps) {
		string wndName = "VertLineContours";
		namedWindow(wndName, WINDOW_NORMAL);
        imshow(wndName, imgResult);
    }

    return output;
}

//vector<Plate> DetectRegions::run(Mat input){
//
//    //Segment image by white
//    vector<Plate> rtn = segmentInVertLine(input);
////    DetectRectangle.setFilename(fileName);
//    vector<Plate> tmp = segmentInRectangle(input);
//
//    //Combile two vector
//    rtn.insert(rtn.end(), tmp.begin(), tmp.end());
//
//    //return detected and posibles regions
//    return rtn;
//}


