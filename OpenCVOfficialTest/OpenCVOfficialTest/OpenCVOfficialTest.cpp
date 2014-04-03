// OpenCVOfficialTest.cpp : Defines the entry point for the console application.
//

// TODO: reference additional headers your program requires here
#include <sstream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>


#define TMAX 100
#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 960
#define MAX_COUNT 400

#include"OpenCVOfficialTest.h"

using namespace cv;
using namespace std;

OpenCVOfficialTest::OpenCVOfficialTest()
{
	this->lowThreshold = 50;
	this->ratio = 3;
	this->windowName = "BOB";
	this->windowNameGray = "Gray";
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	rMax = 255;
	gMax = 255;
	bMax = 255;
	rMin = 0;
	gMin = 0;
	bMin = 0;
	track = false;
	areaToMaximize = 0;
}


#pragma region
void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed





}

void TrackCallBack(int state,  void* userdata)
{
	OpenCVOfficialTest *temp = (OpenCVOfficialTest*)userdata;
}



void ClickedCallBack(int event, int x, int y, int flags, void* userdata)
{
	 	OpenCVOfficialTest *temp = (OpenCVOfficialTest*)userdata;
	//if(temp->frame.empty())
		//return;
	
	if  ( event == EVENT_LBUTTONDOWN )
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		temp->track = true;
	}
else if  ( event == EVENT_RBUTTONDOWN )
{
cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
}
else if  ( event == EVENT_MBUTTONDOWN )
{
cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
}
}


void BarMoved(int i, void * v)
{
	OpenCVOfficialTest *temp = (OpenCVOfficialTest*)v;
	cvtColor(temp->frame, temp->HSV, CV_BGR2GRAY);
	vector<Vec3f> circles;
	blur( temp->HSV, temp->HSV, Size(2,2));
	//Canny( HSV, HSV, lowThreshold, lowThreshold*ratio, 3);
	  /// Apply the Hough Transform to find the circles
    HoughCircles( temp->HSV, circles, CV_HOUGH_GRADIENT, 2, temp->HSV.rows/3, TMAX, 40 + temp->lowThreshold, 40, 150 );
	

	//for( size_t i = 0; i < circles.size(); i++ )
	//{
	  // Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	 //  int radius = cvRound(circles[i][2]);
	if(!circles.empty())
	{
	 Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
	   int radius = cvRound(circles[0][2]);
	   // circle center
	   circle( temp->frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
	   // circle outline
	   circle( temp->frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
	}
	// }
	imshow(temp->windowName, temp->frame);

}
#pragma endregion callback Functions for GUI




#pragma region
void OpenCVOfficialTest::opticalFlow()
{
	//terminates over 20 iterations or when ep is < .03
	TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);

	

	Mat prev, curr, colorImg;
	vector<Point2f> points[2];
	vector<uchar> status;
    vector<float> err;
	points[0].push_back(Point2f((float)1280,(float)960));
	points[1].push_back(Point2f((float)1280,(float)1280));
	 Size winSize(10,10);
	bool needToInit = true;

	while(1)
	{
		capture >> frame;

		if(frame.empty())
			continue;

		namedWindow(windowName, CV_WINDOW_AUTOSIZE );
		cvtColor(frame, curr, CV_BGR2GRAY);
		
		if(needToInit)
		{
			curr.copyTo(prev);
			goodFeaturesToTrack(curr,points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			cornerSubPix(curr, points[0], winSize, Size(-1,-1), termcrit);
			needToInit = false;
			continue;
		}
		calcOpticalFlowPyrLK(prev, curr, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0);

		circle( frame, points[1][0], 20, Scalar(0,0,255), -1, 8);
		imshow(windowName, frame);

		swap(points[1], points[0]);
        swap(prev, curr);	
		waitKey(10);
	}
}
#pragma endregion Optical flow




#pragma region
void OpenCVOfficialTest::trackDemBlobs()
{
	Mat grayImg, colorImg;
	int x = 0;
	int y = 0;
	while(1)
	{
		capture >> frame1;
		  
		if(frame1.empty())
			continue;

		GaussianBlur(frame1, frame1, Size(11,11), 3);

		inRange(frame1,Scalar(rMin,gMin,bMin),Scalar(rMax,gMax,bMax),grayImg);
		  
		Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*MORPH_ELLIPSE + 1, 2*MORPH_ELLIPSE+1 ),
                                       Point( 1, 1 ) );

		erode( grayImg, grayImg, element);
		namedWindow(windowName, CV_WINDOW_AUTOSIZE );
		namedWindow(windowNameGray, CV_WINDOW_AUTOSIZE );
		setMouseCallback(windowName, ClickedCallBack, this);


		if(track)
		{
			findColoredObject(grayImg, x, y);
			drawObject(frame1,x,y);
		}
		imshow(windowName, frame1);
		imshow(windowNameGray, grayImg);
		waitKey(10);
	}
}





void OpenCVOfficialTest::findColoredObject(Mat &grayImg, int &x, int &y)
{
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;
	grayImg.copyTo(temp);
	areaToMaximize = 0;

	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);

		for(int i = 0; i >= 0; i = hierarchy[i][0]) {

			Moments moment = moments((cv::Mat)contours[i]);
			double area = moment.m00;
			//maximization loop
			if(area > areaToMaximize)
			{
			areaToMaximize = area;
			x = moment.m10/area;
			y = moment.m01/area;
			}
		}
}

void OpenCVOfficialTest::drawObject(Mat &frame, int x, int y)
{
	if(x > FRAME_WIDTH || y > FRAME_HEIGHT)
		return;

	Point center(x, y);
	circle( frame, center, 20, Scalar(0,0,255), -1, 8);
}














void OpenCVOfficialTest::BarMovedTest()
{

	while(1)
	{
		capture >> frame;

		if(frame.empty())
			continue;


		namedWindow(windowName, CV_WINDOW_AUTOSIZE );
		createTrackbar( "Min Threshold:", windowName, &lowThreshold, TMAX, BarMoved);
		BarMoved(0,this);
		waitKey(10);
	}
}


void OpenCVOfficialTest::createTrackbars(){
	//create window for trackbars
	string trackbarWindowName = "Track Bars";
	//createButton("Track",TrackCallBack, this, CV_PUSH_BUTTON, 0);
    namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "bMin", bMin);
	sprintf( TrackbarName, "bMax", bMax);
	sprintf( TrackbarName, "rMin", rMin);
	sprintf( TrackbarName, "rMax", rMax);
	sprintf( TrackbarName, "gMin", gMin);
	sprintf( TrackbarName, "gMax", gMax);
    createTrackbar( "bMin", trackbarWindowName, &bMin, bMax, on_trackbar );
    createTrackbar( "bMax", trackbarWindowName, &bMax, bMax, on_trackbar );
    createTrackbar( "rMin", trackbarWindowName, &rMin, rMax, on_trackbar );
    createTrackbar( "rMax", trackbarWindowName, &rMax, rMax, on_trackbar );
    createTrackbar( "gMin", trackbarWindowName, &gMin, gMax, on_trackbar );
    createTrackbar( "gMax", trackbarWindowName, &gMax, gMax, on_trackbar );

}
#pragma endregion color tracking