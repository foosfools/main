#ifndef _OPENCVOFFICIALTEST_H
#define _OPENCVOFFICIALTEST_H


//#include <tchar.h>


//#include "stdafx.h"
// TODO: reference additional headers your program requires here
#include <sstream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#define TMAX 100

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define THRESHOLD 15
#define MAX_COUNT 400

using namespace cv;
using namespace std;

class OpenCVOfficialTest{
	public:
	OpenCVOfficialTest();
	void BarMovedTest();
	void opticalFlow();
	void trackDemBlobs();
	void findColoredObject(Mat &grayImg, int &x, int &y);
	void drawObject(Mat &frame, int x, int y);
	void createTrackbars();

	Mat frame, frame1, HSV;
	int rMin,gMin,bMin;
	int rMax,gMax,bMax;
	VideoCapture capture;
	int lowThreshold;
	int ratio;
	string windowName;
	string windowNameGray;
	bool track;
	int areaToMaximize;

};


#endif


