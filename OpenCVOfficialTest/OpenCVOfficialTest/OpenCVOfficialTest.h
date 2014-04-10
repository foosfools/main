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
#define CENTER_TO_GOAL 60.0/19.0
//number of elements to average over during initialization
#define N_ELEMENTS 10

using namespace cv;
using namespace std;

class OpenCVOfficialTest {
public:
	OpenCVOfficialTest();
	void BarMovedTest();
	void opticalFlow();
	void trackDemBlobs();
	void findColoredObject(Mat &grayImg, int &x, int &y);
	void drawObject(Mat &frame, int x, int y);
	void createTrackbars();
	void Init();
	//nCircles keeps track of how many circles have been counted
	void InitCircle(int & nCircles);
	void FindCorners();
	//nLines keeps track of how many lines have been counted
	void InitLines(int & nLines);
	Mat frame, frame1, HSV;
	int rMin, gMin, bMin;
	int rMax, gMax, bMax;
	VideoCapture capture;
	int lowThreshold;
	int ratio;
	string windowName;
	string windowNameGray;
	bool track;
	int areaToMaximize;
private:
	Vec2f getGoodLine(vector<Vec2f> lines, int &nLines);
	//is false until an average is taken
	bool circleInit;
	bool lineInit;
	//converts a line from polar to cartesian
	Vec4i convertToCartesian(double rho, double theta, int length);
	//list of circles to average during initialization
	Vec3f circleList[N_ELEMENTS];
	//list of lines to average during initialization
	Vec2f lineList[N_ELEMENTS];
	//averages out lines from initLines
	Vec2f AverageOutLines(Vec2f lineList[]);
	//averages out circles from initCircles
	Vec3f AverageOutCircles(Vec3f circleList[]);
};

#endif

