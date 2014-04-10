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

// Setup phase-
// Lighting
// Circle init

OpenCVOfficialTest::OpenCVOfficialTest() {
	this->lowThreshold = 50;
	this->ratio = 3;
	this->windowName = "BOB";
	this->windowNameGray = "Gray";
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	// cout << capture.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
	// cout << capture.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
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
void on_trackbar(int, void*) {	//This function gets called whenever a
								// trackbar position is changed

}

void TrackCallBack(int state, void* userdata) {
	OpenCVOfficialTest *temp = (OpenCVOfficialTest*) userdata;
}

void ClickedCallBack(int event, int x, int y, int flags, void* userdata) {
	OpenCVOfficialTest *temp = (OpenCVOfficialTest*) userdata;
	//if(temp->frame.empty())
	//return;

	if (event == EVENT_LBUTTONDOWN) {
		cout << "Left button of the mouse is clicked - position (" << x << ", "
				<< y << ")" << endl;
		temp->track = true;
	} else if (event == EVENT_RBUTTONDOWN) {
		cout << "Right button of the mouse is clicked - position (" << x << ", "
				<< y << ")" << endl;
	} else if (event == EVENT_MBUTTONDOWN) {
		cout << "Middle button of the mouse is clicked - position (" << x
				<< ", " << y << ")" << endl;
	}
}

#pragma endregion callback Functions for GUI

#pragma region
void OpenCVOfficialTest::opticalFlow() {
	//terminates over 20 iterations or when ep is < .03
	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);

	Mat prev, curr, colorImg;
	vector<Point2f> points[2];
	vector<uchar> status;
	vector<float> err;
	points[0].push_back(Point2f((float) 1280, (float) 960));
	points[1].push_back(Point2f((float) 1280, (float) 1280));
	Size winSize(10, 10);
	bool needToInit = true;

	while (1) {
		capture >> frame;

		if (frame.empty())
			continue;

		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		cvtColor(frame, curr, CV_BGR2GRAY);

		if (needToInit) {
			curr.copyTo(prev);
			goodFeaturesToTrack(curr, points[0], MAX_COUNT, 0.01, 10, Mat(), 3,
					0, 0.04);
			cornerSubPix(curr, points[0], winSize, Size(-1, -1), termcrit);
			needToInit = false;
			continue;
		}
		calcOpticalFlowPyrLK(prev, curr, points[0], points[1], status, err,
				winSize, 3, termcrit, 0);

		circle(frame, points[1][0], 20, Scalar(0, 0, 255), -1, 8);
		imshow(windowName, frame);

		swap(points[1], points[0]);
		swap(prev, curr);
		waitKey(10);
	}
}
#pragma endregion Optical flow

#pragma region

void OpenCVOfficialTest::FindCorners() {

	/// Parameters for Shi-Tomasi algorithm
	vector<Point2f> corners;
	int maxCorners = 2;
	double qualityLevel = 0.01;
	double minDistance = 500;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;

	cvtColor(this->frame, this->HSV, CV_BGR2GRAY);

	/// Apply corner detection
	goodFeaturesToTrack(this->HSV, corners, maxCorners, qualityLevel,
			minDistance, Mat(), blockSize, useHarrisDetector, k);

	int r = 4;
	for (int i = 0; i < corners.size(); i++) {
		circle(this->frame, corners[i], r, Scalar(0, 0, 255), 3, 8, 0);
	}
}

void OpenCVOfficialTest::Init() {

	//keeps count of how many lines and circles have been found
	int nLines = 0;
	int nCircles = 0;
		//loop until both lines and circles are ready
	while(nLines < 10 || nCircles < 10)
	{
		capture >> frame;

		if (frame.empty())
			continue;




		blur(this->HSV, this->HSV, Size(2, 2));
		cvtColor(this->frame, this->HSV, CV_BGR2GRAY);

		if(nCircles < 10)
		InitCircle(nCircles);

		if(nLines < 10)
		InitLines(nLines);






		//display image
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		imshow(this->windowName, this->frame);

		waitKey(10);
	}
}

void OpenCVOfficialTest::InitCircle(int & nCircles) {

	// Find the corners
	//FindCorners();
	vector<Vec3f> circles;
	//initialized once. Keeps track of number of circles to average
	static int circleCount = 0;
	/// Apply the Hough Transform to find the circles
	HoughCircles(this->HSV, circles, CV_HOUGH_GRADIENT, 2, this->HSV.rows / 3,
	TMAX, 40 + this->lowThreshold, 40, 150);

	//for( size_t i = 0; i < circles.size(); i++ )
	//{
	// Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	//  int radius = cvRound(circles[i][2]);
	if (!circles.empty()) {
		Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
		int radius = cvRound(circles[0][2]);
		//add to list of circles
		circleList[nCircles] = circles[0];
		nCircles++;

		// circle center
		circle(this->frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(this->frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}
}

void OpenCVOfficialTest::InitLines(int & nLines) {
	vector<Vec2f> lines;
	Canny(this->HSV, this->HSV, 50, 200, 3);
	HoughLines(this->HSV, lines, 1, .1, 70);

	//find a good line
	Vec2f goodLine = getGoodLine(lines, nLines);
}






Vec2f OpenCVOfficialTest::getGoodLine(vector<Vec2f> lines, int &nLines)
{
	double theta = 0;//CV_PI/2;
	double thresh = (20.0*CV_PI)/180.0;
	Vec2f goodLine;

	//minimizes theta to find a good line
	for (size_t i = 0; i < lines.size(); i++) {

		//if theta is within the threshold, then return the line
		if(abs(lines[i][1] - theta) < thresh)
		{
		goodLine = lines[i];
		//add line to list
		lineList[nLines] = goodLine;
		nLines++;

		Vec4i linePts = this->convertToCartesian(goodLine[0], goodLine[1], 400);
		//draw line to frame
		line(this->frame, Point(linePts[0], linePts[1]), Point(linePts[2], linePts[3]), Scalar(0, 0, 255), 10, CV_AA);
		break;
		}
	}
	return goodLine;
}


#pragma endregion init

#pragma region
void OpenCVOfficialTest::trackDemBlobs() {
	Mat grayImg, colorImg;
	int x = 0;
	int y = 0;
	while (1) {
		capture >> frame1;

		if (frame1.empty())
			continue;

		GaussianBlur(frame1, frame1, Size(11, 11), 3);

		inRange(frame1, Scalar(rMin, gMin, bMin), Scalar(rMax, gMax, bMax),
				grayImg);

		// Pattern to erode with
		Mat element = getStructuringElement(MORPH_ELLIPSE,
				Size(2 * MORPH_ELLIPSE + 1, 2 * MORPH_ELLIPSE + 1),
				Point(1, 1));

		erode(grayImg, grayImg, element);
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		namedWindow(windowNameGray, CV_WINDOW_AUTOSIZE);

		// Starts tracking on click
		setMouseCallback(windowName, ClickedCallBack, this);

		if (track) {
			findColoredObject(grayImg, x, y);
			drawObject(frame1, x, y);
		}
		imshow(windowName, frame1);
		imshow(windowNameGray, grayImg);
		waitKey(10);
	}
}

void OpenCVOfficialTest::findColoredObject(Mat &grayImg, int &x, int &y) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;
	grayImg.copyTo(temp);
	areaToMaximize = 0;

	findContours(temp, contours, hierarchy, CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);
//need this to prevent crash
	if (!hierarchy.size())
		return;

	for (int i = 0; i >= 0; i = hierarchy[i][0]) {

		Moments moment = moments((cv::Mat) contours[i]);
		double area = moment.m00;
		//maximization loop
		if (area > areaToMaximize) {
			areaToMaximize = area;
			x = moment.m10 / area;
			y = moment.m01 / area;
		}
	}
}

void OpenCVOfficialTest::drawObject(Mat &frame, int x, int y) {
	if (x > FRAME_WIDTH || y > FRAME_HEIGHT)
		return;

	Point center(x, y);
	circle(frame, center, 20, Scalar(0, 0, 255), -1, 8);
}

void OpenCVOfficialTest::BarMovedTest() {

	namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	while (1) {
		capture >> frame;

		if (frame.empty())
			continue;

		//createTrackbar( "Min Threshold:", windowName, &lowThreshold, TMAX, InitCircle);
		waitKey(10);
	}
}

void OpenCVOfficialTest::createTrackbars() {
//create window for trackbars
	string trackbarWindowName = "Track Bars";
//createButton("Track",TrackCallBack, this, CV_PUSH_BUTTON, 0);
	namedWindow(trackbarWindowName, 0);
//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "bMin", bMin);
	sprintf(TrackbarName, "bMax", bMax);
	sprintf(TrackbarName, "rMin", rMin);
	sprintf(TrackbarName, "rMax", rMax);
	sprintf(TrackbarName, "gMin", gMin);
	sprintf(TrackbarName, "gMax", gMax);
	createTrackbar("bMin", trackbarWindowName, &bMin, bMax, on_trackbar);
	createTrackbar("bMax", trackbarWindowName, &bMax, bMax, on_trackbar);
	createTrackbar("rMin", trackbarWindowName, &rMin, rMax, on_trackbar);
	createTrackbar("rMax", trackbarWindowName, &rMax, rMax, on_trackbar);
	createTrackbar("gMin", trackbarWindowName, &gMin, gMax, on_trackbar);
	createTrackbar("gMax", trackbarWindowName, &gMax, gMax, on_trackbar);

}
#pragma endregion color tracking

#pragma region
Vec4i OpenCVOfficialTest::convertToCartesian(double rho, double theta, int length)
{
				Point linePt1, linePt2;
				Vec4i tempVec;
				double xSc = cos(theta);
				double ySc = sin(theta);

				double x = xSc * rho;
				double y = ySc * rho;

				theta = theta - CV_PI / 2.0;
				xSc = cos(theta);
				ySc = sin(theta);
				tempVec[0] = cvRound(length * xSc + x);
				tempVec[1]  = cvRound(length * (ySc) + y);

				tempVec[2] = cvRound(-length * xSc + x);
				tempVec[3]  = cvRound(-length * (ySc) + y);

				return tempVec;
}



	//averages out lines from initLines
Vec2f AverageOutLines(Vec2f lineList[])
{
	float sum0 = 0;
	float sum1 = 0;
	Vec2f aveVec;
	for(int i = 0; i < N_ELEMENTS; i++)
	{
		sum0 += lineList[i][0];
		sum1 += lineList[i][1];
	}
	aveVec[0] = sum0/(N_ELEMENTS);
	aveVec[1] = sum1/(N_ELEMENTS);
	return aveVec;
}


	//averages out circles from initCircles
Vec3f AverageOutCircles(Vec3f circleList[])
{
	float sum0 = 0;
	float sum1 = 0;
	float sum2 = 0;
	Vec3f aveVec;
	for(int i = 0; i < N_ELEMENTS; i++)
	{
		sum0 += circleList[i][0];
		sum1 += circleList[i][1];
		sum2 += circleList[i][2];
	}
	aveVec[0] = sum0/(N_ELEMENTS);
	aveVec[1] = sum1/(N_ELEMENTS);
	aveVec[2] = sum2/(N_ELEMENTS);
	return aveVec;
}
#pragma endregion private helper methods