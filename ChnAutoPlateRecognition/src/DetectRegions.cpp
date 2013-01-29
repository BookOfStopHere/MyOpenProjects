/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
*****************************************************************************/

#include "DetectRegions.h"

void DetectRegions::setFilename(string s) {
        filename = s;
}

DetectRegions::DetectRegions(){
    showSteps = false;
    saveRegions = false;
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

    int area= mr.size.height * mr.size.width;
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
Mat DetectRegions::histeq(Mat in)
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

// Segment all possible area of auto plates
vector<Plate> DetectRegions::segment(Mat input){
    vector<Plate> output;

    //convert image to gray
    Mat img_gray;
    cvtColor(input, img_gray, CV_BGR2GRAY);
    blur(img_gray, img_gray, Size(5,5));    

    //Finde vertical lines. Car plates have high density of vertical lines
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

    //Start to iterate to each contour founded
    vector<vector<Point> >::iterator itc= contours.begin();
    vector<RotatedRect> rects;

    //Remove patch that are no inside limits of aspect ratio and area.    
    while (itc != contours.end()) {
        //Create bounding rect of object
        RotatedRect mr = minAreaRect(Mat(*itc));
        if( !verifySizes(mr)){
            itc = contours.erase(itc);
        }else{
            ++itc;
            rects.push_back(mr);

//            //FillPoly
//            Mat imgPoly = Mat::zeros(input.rows, input.cols, CV_8UC1);
//            fillPoly(imgPoly, Mat(*itc), 255);
//            imshow("Fill Poly", imgPoly);
//            cvWaitKey(0);

//            int inRect = pointPolygonTest(Mat(*itc), mr.center, true);
//            cout << "Contours center point(" << mr.center.x << "," << mr.center.y << ") status=" << inRect << "\n";
        }
    }

    //Draw blue contours on a white image
    cv::Mat result;
    input.copyTo(result);
    cv::drawContours(result,contours,
            -1, // draw all contours
            cv::Scalar(255,0,0), // in blue
            1); // with a thickness of 1

//    if(showSteps) {
//    	imshow("Result with Contours", result);
//    	cvWaitKey(0);
//    }

    for(int i=0; i< rects.size(); i++){
        //Draw green minAreaRect with Green
		Point2f vertices[4];
		rects[i].points(vertices);
		for (int n = 0; n < 4; n++)
			line(result, vertices[n], vertices[(n+1)%4], Scalar(0,255,0));	//Green
		//Show the index of rects
		char str1[5];
		sprintf(str1, "%d", i);
		putText(result, str1, vertices[0], CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0), 2);

//		//TODO: Judge if center point is in rect
//		int inRect = pointPolygonTest(Mat(vertices), rects[i].center, false);
//		cout << "minRect center point(" << rects[i].center.x << "," << rects[i].center.y << ") status=" << inRect << "\n";

    	//For better rect cropping for each possible box
        //Make floodfill algorithm because the plate has white background
        //And then we can retrieve the contour box more clearly
        circle(result, rects[i].center, 3, Scalar(0,255,0), -1);	//Green for center point

        //get the min and max size between width and height
        float minSize=(rects[i].size.width < rects[i].size.height)? rects[i].size.width: rects[i].size.height;
        minSize = minSize - minSize*0.5;

        float maxSize=(rects[i].size.width > rects[i].size.height)? rects[i].size.width: rects[i].size.height;
        maxSize = maxSize / 6;
        Point ptSample;
        if(rects[i].size.width > rects[i].size.height) {
            ptSample.x = rects[i].center.x - maxSize * cos(rects[i].angle);
            ptSample.y = rects[i].center.y - maxSize * sin(rects[i].angle);
        } else {
            ptSample.x = rects[i].center.x - maxSize * sin(rects[i].angle);
            ptSample.y = rects[i].center.y - maxSize * cos(rects[i].angle);
        }
        circle(result, ptSample, 3, Scalar(0,255,255), -1);	//Yellow for sample point

        if(showSteps) {
			cout << "GREEN Rectangle[" << i << "] region of possible plate: \n";
			cout << "center point x=" << rects[i].center.x << ", y=" << rects[i].center.y << "\n"
					<< "sample point x=" << ptSample.x << ", y=" << ptSample.y << "\n"
					<< "width=" << rects[i].size.width << ", height=" << rects[i].size.height
					<< ", angle=" << rects[i].angle << "\n";
        }


//        //show four point of rects[i]
//        Point2f vertices[4];
//        rects[i].points(vertices);
//        for (int n = 0; n < 4; n++)
//        	cout << "Possible region rect[" << i << "] of plate: "
//        			<< "x=" << vertices[n].x << ", y=" << vertices[n].y << "\n";
//            line(image, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));

        //
        //initialize rand and get 10 points around center for floodfill algorithm
        srand ( time(NULL) );
        //Initialize floodfill parameters and variables
        Mat mask;
        mask.create(input.rows + 2, input.cols + 2, CV_8UC1);
        mask = Scalar::all(0);
        int loDiff = 30;
        int upDiff = 30;
        //TODO: Set the mask value and connectivity
        int connectivity = 4;	//4
        int newMaskVal = 255;	//255
        int flags = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
//        int flags = connectivity + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
        int flags2 = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE;
//        int flags2 = connectivity + CV_FLOODFILL_FIXED_RANGE;
        int NumSeeds = 10;	//10
        Rect ccomp;
        cv::Mat input2;
        for(int j = 0; j < NumSeeds; j++) {
            Point seed;
            seed.x = rects[i].center.x + rand()%(int)minSize - (minSize/2);
            seed.y = rects[i].center.y + rand()%(int)minSize - (minSize/2);
//TODO: To set the seed point properly
//	pointPolygonTest
//            seed.x = rects[i].center.x - rects[i].size.width / 3;
//            seed.y = rects[i].center.y - rects[i].size.height / 3;

            circle(result, seed, 1, Scalar(0,0,255), -1);	//Draw seed point with Red

            int area = floodFill(input, mask, seed, Scalar(0,255,255), &ccomp,	//Yellow
            				Scalar(loDiff, loDiff, loDiff), Scalar(upDiff, upDiff, upDiff), flags);

            //TODO: Show the result of floodfilling
            input.copyTo(input2);
            int area2 = floodFill(input2, seed, Scalar(0,255,255), &ccomp,
            				Scalar(loDiff, loDiff, loDiff), Scalar(upDiff, upDiff, upDiff), flags2);
        }

//        if(showSteps) {
//            imshow("FloodFillMask", mask);
//        	imshow("FloodFill", input2);
//            cvWaitKey(0);
//        }

        //Check new floodfill mask match for a correct patch.
        //Get all points detected for get Minimal rotated Rect
        vector<Point> pointsInterest;
        Mat_<uchar>::iterator itMask= mask.begin<uchar>();
        Mat_<uchar>::iterator end= mask.end<uchar>();
        for( ; itMask!=end; ++itMask)
        	//TODO: if the point is white???
            if(*itMask==255)
                pointsInterest.push_back(itMask.pos());

        RotatedRect minRect = minAreaRect(pointsInterest);

        if(verifySizes(minRect)){
            //Drawing the rotated rectangle
            Point2f rect_points[4];
            minRect.points( rect_points );

//            //Show the parameters of RotatedRect
//            if(showSteps) {
//				cout << "RED Rectangle region of possible plate: \n";
//				cout << "\tcenter of x=" << minRect.center.x << ", y=" << minRect.center.y
//						<< ", width=" << minRect.size.width << ", height=" << minRect.size.height
//						<< ", angle=" << minRect.angle << "\n";
//            }

            //Draw the RotatedRect with Red
            for( int j = 0; j < 4; j++ ) {
                line( result, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255), 1, 8 );    
//				cout << "\tvertex[" << j << "] x=" << rect_points[j].x << ", y=" << rect_points[j].y << "\n";
            }

            //Get rotation matrix ???
            float r = (float)minRect.size.width / (float)minRect.size.height;
            float angle = minRect.angle;
            if(r < 1)
                angle = 90 + angle;
            Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);

            //Create and rotate image
            Mat img_rotated;
            warpAffine(input, img_rotated, rotmat, input.size(), CV_INTER_CUBIC);

            //Crop image
            Size rect_size = minRect.size;
            if(r < 1)
                swap(rect_size.width, rect_size.height);
            Mat img_crop;
            getRectSubPix(img_rotated, rect_size, minRect.center, img_crop);
            
            Mat resultResized;
            // TODO: create the image with the size of Chinese Auto Plate
//            resultResized.create(33, 144, CV_8UC3);	//for Spain
            resultResized.create(35, 110, CV_8UC3);	//for China

            resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
            //Equalize croped image
            Mat grayResult;
            cvtColor(resultResized, grayResult, CV_BGR2GRAY); 
            blur(grayResult, grayResult, Size(3,3));
            grayResult = histeq(grayResult);

            // Save the blured gray image to file
            if(saveRegions){ 
                stringstream ss(stringstream::in | stringstream::out);
                ss << "../tmp/" << filename << "_" << i << ".jpg";
                imwrite(ss.str(), grayResult);
            }

            output.push_back(Plate(grayResult,minRect.boundingRect()));
        }
    }       

    if(showSteps) 
        imshow("Contours", result);

    return output;
}

vector<Plate> DetectRegions::run(Mat input){
    
    //Segment image by white 
    vector<Plate> tmp = segment(input);

    //return detected and posibles regions
    return tmp;
}


