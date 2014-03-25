// OpenCVOfficialTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <tchar.h>



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
using namespace cv;
using namespace std;

	Mat frame, HSV;
	VideoCapture capture;
	int lowThreshold = 50;
	int ratio = 3;
	string windowName = "BOB";

void BarMovedTest();
void BarMoved(int i, void * v);
void opticalFlow();

int _tmain(int argc, _TCHAR* argv[])
{
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	
	opticalFlow();
	

	return 0;
}















void opticalFlow()
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




void BarMovedTest()
{

	while(1)
	{
		capture >> frame;

		if(frame.empty())
			continue;

		 
		namedWindow(windowName, CV_WINDOW_AUTOSIZE );
		createTrackbar( "Min Threshold:", windowName, &lowThreshold, TMAX, BarMoved);
		BarMoved(0,0);
		waitKey(10);
	}
}



void BarMoved(int i, void * v)
{


	cvtColor(frame, HSV, CV_BGR2GRAY);
	vector<Vec3f> circles;
	blur( HSV, HSV, Size(2,2));
	//Canny( HSV, HSV, lowThreshold, lowThreshold*ratio, 3);
	  /// Apply the Hough Transform to find the circles
    HoughCircles( HSV, circles, CV_HOUGH_GRADIENT, 2, HSV.rows/3, TMAX, 40 + lowThreshold, 40, 150 );
	

	//for( size_t i = 0; i < circles.size(); i++ )
	//{
	  // Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	 //  int radius = cvRound(circles[i][2]);
	if(!circles.empty())
	{
	 Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
	   int radius = cvRound(circles[0][2]);
	   // circle center
	   circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
	   // circle outline
	   circle( frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
	}
	// }
	imshow(windowName, frame);

}
