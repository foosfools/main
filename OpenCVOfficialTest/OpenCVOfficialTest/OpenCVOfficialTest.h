#ifndef _OPENCVOFFICIALTEST_H
#define _OPENCVOFFICIALTEST_H

//#include <tchar.h>

//#include "stdafx.h"
// TODO: reference additional headers your program requires here
/*rmin, gmin, bmin: 19, 49, 135
rmax, gmax, bmax: 93, 120, 246
 *
 */
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
#include "Board.h"

#define TMAX 100

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define THRESHOLD 15
#define MAX_COUNT 400
#define CENTER_TO_GOAL 60.0/19.0
//number of elements to average over during initialization
#define N_ELEMENTS 30

using namespace cv;
using namespace std;

class OpenCVOfficialTest {
public:
	OpenCVOfficialTest();
	void BarMovedTest();
	void testOpticalFlow();
	void opticalFlow();
	void trackDemBlobs();
	void findColoredObject(Mat &grayImg, int &x, int &y);
	void drawObject(Mat &frame, int x, int y);
	void createTrackbars();
	//initializes board and ball
	void Init();
	//tracks ball
	void TrackBall();
	//nCircles keeps track of how many circles have been counted
	void InitCircle(int & nCircles);
	void FindCorners();
	//nLines keeps track of how many lines have been counted
	void InitLines(int & nLines);
	//initializes ball based off of color
	void InitBall(int & nBalls);
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
	Point2f point;
	bool addRemovePt;
	Board board;
private:
	Vec2f getGoodLine(vector<Vec2f> lines, int &nLines);
	//average values from circles and lines from initialization
	Vec3f aveCircle;
	Vec2f aveLine;
	Point aveBall;
	//x1, y1, x2, y2
	Vec4f goaliePos;
	bool lineInit;
	//converts a line from polar to cartesian
	Vec4i convertToCartesian(double rho, double theta, int length);
	//list of circles to average during initialization
	Vec3f circleList[N_ELEMENTS];
	//list of lines to average during initialization
	Vec2f lineList[N_ELEMENTS];
	//list of balls to average
	Point ballList[N_ELEMENTS];
	//averages out lines from initLines
	Vec2f averageOutLines();
	//averages out circles from initCircles
	Vec3f averageOutCircles();
	//verages out ball coords from initBalls
	Point averageOutBalls();
	void calcGoalPosition(Vec2f line, Vec3f circle);
};

#endif
