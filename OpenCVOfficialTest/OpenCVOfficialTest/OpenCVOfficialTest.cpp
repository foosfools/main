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
#include <ctime>
#include <unistd.h>

#define TMAX 100
#define MAX_COUNT 400
#define DISPLAY_WINDOWS true

#include "OpenCVOfficialTest.h"
#include "Board.h"
#include "math.h"
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
	this->addRemovePt = false;
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	// cout << capture.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
	// cout << capture.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

//	int minArr[3] = { 0, 94, 179};
//	int maxArr[3] = { 85, 124, 225};
// 	int  minArr[3] = {0, 70, 199};
// 	int maxArr[3] = {38, 223, 255};
	int minArr[3] = {0, 85, 141};
	int maxArr[3] = {81, 160, 255};

	rMax = maxArr[0];
	gMax = maxArr[1];
	bMax = maxArr[2];
	rMin = minArr[0];
	gMin = minArr[1];
	bMin = minArr[2];
	track = false;
	areaToMaximize1 = 0;
	//////////LOW LIGHT 920 ///////////////
	//int minArr[3] = { 39, 46, 136 };
	//int maxArr[3] = { 77, 104, 255 };
	//
	//rmin, gmin, bmin: 66, 59, 165
	//rmax, gmax, bmax: 93, 120, 246
	///////////Bar buffers/////////////////
	//rmin, gmin, bmin: 0, 162, 0
	//rmax, gmax, bmax: 128, 255, 202
	///////////Green Bar buffers/////////////////
	//rmin, gmin, bmin: 76, 142, 0
	//rmax, gmax, bmax: 119, 167, 227
	//controller();
	
	uart_fd = virtualPort_open(); 
	configure_port(uart_fd); 
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

void onMouse(int event, int x, int y, int /*flags*/, void* userdata) {
	OpenCVOfficialTest *temp = (OpenCVOfficialTest*) userdata;
	if (event == CV_EVENT_LBUTTONDOWN) {
		temp->point = Point2f((float) x, (float) y);
		temp->addRemovePt = true;
	}
}

#pragma endregion callback Functions for GUI

#pragma region

void OpenCVOfficialTest::testOpticalFlow() {
	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
	Size subPixWinSize(10, 10), winSize(31, 31);
	bool needToInit = false;

	namedWindow("LK Demo", 1);
	setMouseCallback("LK Demo", onMouse, this);

	Mat gray, prevGray, image;
	vector<Point2f> points[2];

	while (1) {
		capture >> frame1;
		if (frame1.empty()) {
			continue;
		}

		frame1.copyTo(image);
		cvtColor(image, gray, COLOR_BGR2GRAY);

		if (needToInit) {
			// automatic initialization
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3,
					0, 0.04);
			cornerSubPix(gray, points[1], subPixWinSize, Size(-1, -1),
					termcrit);
			addRemovePt = false;
		} else if (!points[0].empty()) {
			vector<uchar> status;
			vector<float> err;
			if (prevGray.empty())
				gray.copyTo(prevGray);
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status,
					err, winSize, 3, termcrit, 0, 0.001);
			size_t i, k;
			for (i = k = 0; i < points[1].size(); i++) {
				if (addRemovePt) {
					if (norm(point - points[1][i]) <= 5) {
						addRemovePt = false;
						continue;
					}
				}

				if (!status[i])
					continue;

				points[1][k++] = points[1][i];
				circle(image, points[1][i], 3, Scalar(0, 255, 0), -1, 8);
			}
			points[1].resize(k);
		}

		if (addRemovePt && points[1].size() < (size_t) MAX_COUNT) {
			vector<Point2f> tmp;
			tmp.push_back(point);
			cornerSubPix(gray, tmp, winSize, cvSize(-1, -1), termcrit);
			points[1].push_back(tmp[0]);
			addRemovePt = false;
		}

		needToInit = false;
		if(DISPLAY_WINDOWS){
			imshow("LK Demo", image);
		}
		char c = (char) waitKey(10);
		if (c == 27)
			break;
		switch (c) {
			case 'r':
				needToInit = true;
				break;
			case 'c':
				points[0].clear();
				points[1].clear();
				break;
		}

		std::swap(points[1], points[0]);
		cv::swap(prevGray, gray);
	}
}
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
		if(DISPLAY_WINDOWS){
			imshow(windowName, frame);
		}
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
	int nBalls = 0;
	bool circlesInit = false;
	bool linesInit = false;
	bool ballInit = false;
	//loop until both lines and circles are ready
	while (nBalls < N_ELEMENTS){//(nLines < N_ELEMENTS || nCircles < N_ELEMENTS || nBalls < N_ELEMENTS) {
		capture >> frame;

		if (frame.empty())
			continue;

		//blur(this->frame, this->frame, Size(2, 2));
		cvtColor(this->frame, this->HSV, CV_BGR2GRAY);
/*
		if (nCircles < N_ELEMENTS)
			InitCircle(nCircles);
		else{
			if(!circlesInit)
				cout << "Circles initialized" << endl;
			circlesInit = true;
		}
		if (nLines < N_ELEMENTS)
			InitLines(nLines);
		else{
			if(!linesInit)
				cout << "Lines initialized" << endl;
			linesInit = true;
		}*/
		if (nBalls < N_ELEMENTS)
		{
			GaussianBlur(frame,frame, Size(5,5),2);
			InitBall(nBalls);
		}
		else{
			if(!ballInit)
				cout << "Ball initialized" << endl;
			ballInit = true;
		}
		if (DISPLAY_WINDOWS) {
			//display image
			namedWindow(windowName, CV_WINDOW_AUTOSIZE);
			imshow(this->windowName, this->frame);
		}

		waitKey(10);
	}
	//get average circle and ine dimensions
	aveBall = averageOutBalls();
	aveCircle = averageOutCircles();
	aveLine = averageOutLines();
	//set board parameters
	board.currX = aveBall.x;
	board.currY = aveBall.y;

	// << "theta " << aveLine[1] << endl;

	//while (1) {

	Vec4i tempLine = convertToCartesian(200.0, aveLine[1], 200);
	line(this->frame, Point(tempLine[0], tempLine[1]),
			Point(tempLine[2], tempLine[3]), Scalar(0, 0, 255), 10, CV_AA);
	circle(this->frame, Point(cvRound(aveCircle[0]), cvRound(aveCircle[1])),
			cvRound(aveCircle[2]), Scalar(0, 0, 255), 3, 8, 0);
	drawObject(frame, aveBall.x, aveBall.y, 255, 0, 0);
	//display image
	//calcGoalPosition(aveLine, aveCircle);
	if (DISPLAY_WINDOWS) {
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		setMouseCallback(windowName, ClickedCallBack, this);
		imshow(this->windowName, this->frame);
	}
	waitKey(10);
	//}
}

void OpenCVOfficialTest::InitWithOutBall() {

	//keeps count of how many lines and circles have been found
	int nLines = 0;
	int nCircles = 0;
	//loop until both lines and circles are ready
	while (nLines < N_ELEMENTS || nCircles < N_ELEMENTS) {
		capture >> frame;

		if (frame.empty())
			continue;

		blur(this->frame, this->frame, Size(2, 2));
		cvtColor(this->frame, this->HSV, CV_BGR2GRAY);

		if (nCircles < N_ELEMENTS)
			InitCircle(nCircles);

		if (nLines < N_ELEMENTS)
			InitLines(nLines);

		if (DISPLAY_WINDOWS) {
			//display image
			namedWindow(windowName, CV_WINDOW_AUTOSIZE);
			imshow(this->windowName, this->frame);
		}
		waitKey(10);
	}
	//get average circle and ine dimensions
	aveCircle = averageOutCircles();
	aveLine = averageOutLines();
	//cout << "theta " << aveLine[1] << endl;

	while (1) {

		Vec4i tempLine = convertToCartesian(200.0, aveLine[1], 200);
		line(this->frame, Point(tempLine[0], tempLine[1]),
				Point(tempLine[2], tempLine[3]), Scalar(0, 0, 255), 10, CV_AA);
		circle(this->frame, Point(cvRound(aveCircle[0]), cvRound(aveCircle[1])),
				cvRound(aveCircle[2]), Scalar(0, 0, 255), 3, 8, 0);

		calcGoalPosition(aveLine, aveCircle);

		if (DISPLAY_WINDOWS) {
			//display image
			namedWindow(windowName, CV_WINDOW_AUTOSIZE);
			imshow(this->windowName, this->frame);
		}
		waitKey(10);
	}
}

void OpenCVOfficialTest::calcGoalPosition(Vec2f myLine, Vec3f circle) {
	//create a unit vector
	float theta = myLine[1] + CV_PI / 2;
	Vec2f unitVec(sin(theta), -cos(theta));

	int diam = cvRound(circle[2]) * 2;
	int scalingFactor = (diam * 60) / 20;

	Point goaliePos;
	goaliePos.x = scalingFactor * unitVec[0] + cvRound(circle[0]);
	goaliePos.y = scalingFactor * unitVec[1] + cvRound(circle[1]);

	updateGoalieBarPosition(goaliePos.x, goaliePos.y);

	drawObject(frame, goaliePos.x, goaliePos.y, 255, 0, 0);

}

void OpenCVOfficialTest::updateGoalieBarPosition(int x, int y) {
	Mat tempHSV, tempFrame, grayImg;

	tempFrame = frame(Range::all(), Range(x - 50, frame.cols));

	tempHSV = HSV(Range::all(), Range(x - 50, HSV.cols));

	vector<Vec4i> lines;

	//Canny(tempHSV, tempHSV, 50, 200, 3);
	HoughLinesP(tempHSV, lines, 1, CV_PI / 180, 50, FRAME_HEIGHT / 2, 500);

	//for (size_t i = 0; i < lines.size(); i++) {
	Vec4i l = lines[0];
	//	line(tempFrame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3,CV_AA);
	//}

	inRange(tempFrame, Scalar(GREEN_MIN), Scalar(GREEN_MAX),
			grayImg);

	int x1, y1, x2, y2;
	x1 = 0;
	y1 = 0;
	x2 = 0;
	y2 = 0;
	while ((x1 == 0 && y1 == 0) || (x2 == 0 && y2 == 0)) {
		findColoredObjects2(grayImg, x1, y1, x2, y2);
	}
	board.lastGoaliePos[0] = (x1 + x2) / 2;
	board.lastGoaliePos[1] = (y1 + y2) / 2;
	board.lastGoaliePos[0] += x - 50;
	//	drawObject (frame, board.lastGoaliePos[0], board.lastGoaliePos[1], 0,255, 0);

	//	drawObject (frame, x2 + x - 50, y2, 0, 255, 0);

	//createTrackbars();
	//trackDemBlobs();

}

void OpenCVOfficialTest::getLines(int x, int y, int rowStart, int rowEnd,
		int colStart, int colEnd) {
	Mat tempHSV, tempFrame, grayImg;

	tempFrame = frame(Range::all(), Range(x - 50, frame.cols));

	tempHSV = HSV(Range::all(), Range(x - 50, HSV.cols));

	vector<Vec4i> lines;

	//Canny(tempHSV, tempHSV, 50, 200, 3);
	HoughLinesP(tempHSV, lines, 1, CV_PI / 180, 50, FRAME_HEIGHT / 2, 500);

	//for (size_t i = 0; i < lines.size(); i++) {
	Vec4i l = lines[0];
	//	line(tempFrame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3,CV_AA);
	//}

	//	drawObject (frame, board.lastGoaliePos[0], board.lastGoaliePos[1], 0,255, 0);

	//	drawObject (frame, x2 + x - 50, y2, 0, 255, 0);

	//createTrackbars();
	//trackDemBlobs();

}

void OpenCVOfficialTest::InitCircle(int & nCircles) {

	// Find the corners
	//FindCorners();
	vector<Vec3f> circles;

	/// Apply the Hough Transform to find the circles
	HoughCircles(this->HSV, circles, CV_HOUGH_GRADIENT, 2, this->HSV.rows / 3,
			TMAX, 40 + this->lowThreshold, 40, 150);

	int minCircle = 0;
	double distance = 30;
	double tempDistance = 0.0;

	for (int i = 0; i < circles.size(); i++) {
		double xdist = (FRAME_WIDTH / 2) - circles[i][0];
		double ydist = (FRAME_HEIGHT / 2) - circles[i][1];

		tempDistance = sqrt(abs(pow(xdist, 2.0) - pow(ydist, 2.0)));
		if (tempDistance < distance) {
			distance = tempDistance;
			minCircle = i;
		}
	}

	//for( size_t i = 0; i < circles.size(); i++ )
	//{
	// Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	//  int radius = cvRound(circles[i][2]);
	if (!circles.empty()) {
		Point center(cvRound(circles[minCircle][0]),
				cvRound(circles[minCircle][1]));
		int radius = cvRound(circles[minCircle][2]);

		//add to list of circles
		circleList[nCircles] = circles[minCircle];
		nCircles++;
		//cout << "c:" << nCircles << endl;
		// circle center
		circle(this->frame, center, radius - nCircles,
				Scalar(0, 255 - nCircles * 10, 0), -1, 8, 0);
		// circle outline
		//	circle(this->frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}
}

void OpenCVOfficialTest::InitLines(int & nLines) {
	vector<Vec2f> lines;
	Canny(this->HSV, this->HSV, 50, 200, 3);
	HoughLines(this->HSV, lines, 1, .05, 70);

	//find a good line
	Vec2f goodLine = getGoodLine(lines, nLines);
}

Vec2f OpenCVOfficialTest::getGoodLine(vector<Vec2f> lines, int &nLines) {
	double theta = 0;	//CV_PI/2;
	double thresh = (20.0 * CV_PI) / 180.0;
	Vec2f goodLine;

	//minimizes theta to find a good line
	for (size_t i = 0; i < lines.size(); i++) {

		//if theta is within the threshold, then return the line
		if (abs(lines[i][1] - theta) < thresh) {
			goodLine = lines[i];
			//add line to list
			lineList[nLines] = goodLine;
			nLines++;

			Vec4i linePts = this->convertToCartesian(goodLine[0], goodLine[1],
					400);
			//draw line to frame
			//line(this->frame, Point(linePts[0], linePts[1]), Point(linePts[2], linePts[3]), Scalar(0, 0, 255), 10, CV_AA);
			break;
		}
	}
	return goodLine;
}

void OpenCVOfficialTest::InitBall(int &nBalls) {
	Mat grayImg, colorImg;
	int x = 0;
	int y = 0;

	frame.copyTo(frame1);

	inRange(frame1, Scalar(rMin, gMin, bMin), Scalar(rMax, gMax, bMax),
			grayImg);

	// Pattern to erode with
	Mat element = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * MORPH_ELLIPSE + 1, 2 * MORPH_ELLIPSE + 1), Point(1, 1));

	//erode(grayImg, grayImg, element);
	findColoredObject(grayImg, x, y);
	if (x != 0 || y != 0) {
		ballList[nBalls] = Point(x, y);
		nBalls++;
	}
	//drawObject(frame, x, y);
}

#pragma endregion init

#pragma region

//performs color tracking
void OpenCVOfficialTest::IdentifyBall() {
	Mat grayImg;
	inRange(frame, Scalar(rMin, gMin, bMin), Scalar(rMax, gMax, bMax), grayImg);
	// Pattern to erode with
	Mat element = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * MORPH_ELLIPSE + 1, 2 * MORPH_ELLIPSE + 1), Point(1, 1));

	//erode(grayImg, grayImg, element);
	findColoredObject(grayImg, board.currX, board.currY);
}



void OpenCVOfficialTest::trackDemBlobs() {
	Mat grayImg, colorImg;
	int x = 0;
	int y = 0;
	static bool isPrinted = false;
	while (1) {
		capture >> frame1;

		if (frame1.empty())
			continue;

		blur(frame1, frame1, Size(2, 2));

		inRange(frame1, Scalar(rMin, gMin, bMin), Scalar(rMax, gMax, bMax),
				grayImg);

		// Pattern to erode with
		Mat element = getStructuringElement(MORPH_ELLIPSE,
				Size(2 * MORPH_ELLIPSE + 1, 2 * MORPH_ELLIPSE + 1),
				Point(1, 1));

		// Starts tracking on click
		setMouseCallback(windowName, ClickedCallBack, this);

		if (track) {
			findColoredObject(grayImg, x, y);
			drawObject(frame1, x, y, 255, 0, 0);
			if (!isPrinted) {
				isPrinted = true;
				cout << "rmin, gmin, bmin: " << rMin << ", " << gMin << ", "
					<< bMin << endl;
				cout << "rmax, gmax, bmax: " << rMax << ", " << gMax << ", "
					<< bMax << endl;
			}

		}

		if (DISPLAY_WINDOWS) {
			//erode(grayImg, grayImg, element);
			namedWindow(windowName, CV_WINDOW_AUTOSIZE);
			namedWindow(windowNameGray, CV_WINDOW_AUTOSIZE);
			imshow(windowName, frame1);
			imshow(windowNameGray, grayImg);
		}
		waitKey(10);
	}
}

void OpenCVOfficialTest::findColoredObject(Mat &grayImg, int &x, int &y) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;
	grayImg.copyTo(temp);
	areaToMaximize1 = 0;

	findContours(temp, contours, hierarchy, CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);
	//need this to prevent crash
	if (!hierarchy.size())
		return;

	for (int i = 0; i >= 0; i = hierarchy[i][0]) {

		Moments moment = moments((cv::Mat) contours[i]);
		double area = moment.m00;
		//maximization loop
		if (area > areaToMaximize1) {
			areaToMaximize1 = area;
			x = moment.m10 / area;
			y = moment.m01 / area;
		}
	}
}

void OpenCVOfficialTest::findColoredObjects2(Mat &grayImg, int &x1, int &y1,
		int &x2, int &y2) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;
	grayImg.copyTo(temp);
	areaToMaximize1 = 0;

	findContours(temp, contours, hierarchy, CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);
	//need this to prevent crash
	if (!hierarchy.size())
		return;

	for (int i = 0; i >= 0; i = hierarchy[i][0]) {

		Moments moment = moments((cv::Mat) contours[i]);
		double area = moment.m00;
		//maximization loop
		if (area > areaToMaximize1) {
			x2 = x1;
			y2 = y1;
			areaToMaximize1 = area;
			x1 = moment.m10 / area;
			y1 = moment.m01 / area;
		}
	}
}

void OpenCVOfficialTest::drawObject(Mat &frame, int x, int y, double blue,
		double green, double red) {
	if (x > FRAME_WIDTH || y > FRAME_HEIGHT)
		return;

	Point center(x, y);
	circle(frame, center, 20, Scalar(blue, green, red), -1, 8);
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

void OpenCVOfficialTest::kalmanFilterInit()
{
	KF = KalmanFilter(4,2,0);

	
	KF.transitionMatrix = *(Mat_<float>(6, 6) << 1,0,1,0,0.5,0, 0,1,0,1,0,0.5, 0,0,1,0,1,0, 0,0,0,1,0,1, 0,0,0,0,1,0, 0,0,0,0,0,1);
	KF.measurementMatrix = *(Mat_<float>(2, 6) << 1,0,1,0,0.5,0, 0,1,0,1,0,0.5);
	//Mat_<float> measurement(2,1); measurement.setTo(Scalar(0));
 
	// init...
	KF.statePre.at<float>(0) = 0;//mouse_info.x;
	KF.statePre.at<float>(1) = 0;//mouse_info.y;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;
	KF.statePre.at<float>(4) = 0;
	KF.statePre.at<float>(5) = 0;
	//setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(KF.errorCovPost, Scalar::all(.1));
}



Point OpenCVOfficialTest::getKalmanPoint(Point measuredPoint)
{
	Mat_<float> measurement(2,1); measurement.setTo(Scalar(0));
	// First predict, to update the internal statePre variable
	Mat prediction = KF.predict();
	Point predictPt(prediction.at<float>(0), prediction.at<float>(1));
			 
	// Get mouse point
	measurement(0) = measuredPoint.x;
	measurement(1) = measuredPoint.y;
			 
	Point measPt(measurement(0),measurement(1));
 
	// The "correct" phase that is going to use the predicted value and our measurement
	Mat estimated = KF.correct(measurement);
	Point statePt(estimated.at<float>(0),estimated.at<float>(1));
	
	return statePt;
}

#pragma endregion filtering




#pragma region
Vec4i OpenCVOfficialTest::convertToCartesian(double rho, double theta,
		int length) {
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
	tempVec[1] = cvRound(length * (ySc) + y);

	tempVec[2] = cvRound(-length * xSc + x);
	tempVec[3] = cvRound(-length * (ySc) + y);

	return tempVec;
}

//averages out lines from initLines
Vec2f OpenCVOfficialTest::averageOutLines() {
	float sum0 = 0;
	float sum1 = 0;
	Vec2f aveVec;
	for (int i = 0; i < N_ELEMENTS; i++) {
		sum0 += lineList[i][0];
		sum1 += lineList[i][1];
	}
	aveVec[0] = sum0 / (N_ELEMENTS);
	aveVec[1] = sum1 / (N_ELEMENTS);
	return aveVec;
}

//averages out circles from initCircles
Vec3f OpenCVOfficialTest::averageOutCircles() {
	float sum0 = 0;
	float sum1 = 0;
	float sum2 = 0;
	Vec3f aveVec;
	for (int i = 0; i < N_ELEMENTS; i++) {
		sum0 += circleList[i][0];
		sum1 += circleList[i][1];
		sum2 += circleList[i][2];
		//cout << "vec:" << sum0 << "  " << sum1 << endl;
	}
	aveVec[0] = sum0 / (N_ELEMENTS);
	aveVec[1] = sum1 / (N_ELEMENTS);
	aveVec[2] = sum2 / (N_ELEMENTS);
	return aveVec;
}

Point OpenCVOfficialTest::averageOutBalls() {
	int sumx = 0;
	int sumy = 0;
	Point avePoint;

	for (int i = 0; i < N_ELEMENTS; i++) {
		sumx += ballList[i].x;
		sumy += ballList[i].y;
	}
	avePoint.x = cvRound((float) sumx / (float) (N_ELEMENTS));
	avePoint.y = cvRound((float) sumy / (float) (N_ELEMENTS));
	return avePoint;
}

void OpenCVOfficialTest::TrackBall() {
	while (1) {
		clock_t startTime = clock();
		capture >> frame;

		if (frame.empty())
			continue;

		GaussianBlur(frame, frame, Size(5,5), 2);


		//sets globals for ball
		IdentifyBall();

		//update ball params if the ball is identified
		board.updateBallVelocity();
		double lastXVel = 0.0;
		Vec2i ballOnRodComp = board.getBallPredictionOnRod(board.rod1);
		Vec2i avgballOnRodComp = board.avgBallOnRod(ballOnRodComp, &lastXVel);
		
		if(avgballOnRodComp[0] != -1)
		{
			Vec2i temp;
			double goalieBallArrivalTime_s = 0.0;
			
			int motor_rod = board.convertRodtoEncoderVal(board.rod1[1][1]);
			board.rod1[1][1] = avgballOnRodComp[1]; 
			
			uint8_t bufSize = 10;
			char outBufA[bufSize];
			char outBufB[bufSize];
			this->createMotorCommand(motor_rod, 0, outBufA);
			write(uart_fd, outBufA, strlen(outBufA));

			goalieBallArrivalTime_s = board.currX / lastXVel;
			
			if(goalieBallArrivalTime_s < 0 && goalieBallArrivalTime_s >= -0.1)
			{
				this->createMotorCommand(-800, 0, outBufB);
				write(uart_fd, outBufB, strlen(outBufB));
			}
			
			cout << "motor pulse: " << motor_rod << endl; 
			cout << "goalieBallArrivalTime_s " << goalieBallArrivalTime_s << endl; 
			circle(frame, Point(avgballOnRodComp[0], avgballOnRodComp[1]), 10, Scalar(0, 255, 0), 3, 8, 0);
		}
		
		Point point2 = Point(
				board.currX + board.lastXComp * board.lastBallVelocity * 50,
				board.currY + board.lastYComp * board.lastBallVelocity * 50);
		line(frame, Point(board.currX, board.currY), point2, Scalar(0, 255, 0),
				3);
		drawObject(frame, board.currX, board.currY, 0, 0, 255);

		if (DISPLAY_WINDOWS) {
			line(frame, Point(board.rod1[0][0], board.rod1[0][1]),
			Point(board.rod1[2][0], board.rod1[2][1]), Scalar(0, 0, 255), 10, CV_AA);
			imshow(windowName, frame);
		}
		
		clock_t endTime = clock();
		clock_t clockTicksTaken = endTime - startTime;
		double fps = (double) (CLOCKS_PER_SEC / clockTicksTaken);

		waitKey(10);
	}
}


//"1:1000"
void OpenCVOfficialTest::createMotorCommand(int motorPulse, int motorNum, char* outBuf)
{
	int motorNum_index = 2;
	
	int index = 0;
	
	outBuf[index++] = motorNum + '0';
	outBuf[index++] = ':';
	
	if(motorPulse < 0)
	{
		outBuf[index++] = '-';
		motorPulse = -motorPulse;
		 motorNum_index = 3;
	}
	
	for(int i = motorPulse; i != 0; i /= 10)
	{
		outBuf[index++] = (i % 10) + '0';
	}
	
	int swapLen = index - motorNum_index;
	
	for(int j = 0; j < swapLen / 2; j++)
	{
		int temp = outBuf[motorNum_index + j];
		outBuf[motorNum_index + j] = outBuf[index - j - 1];
		outBuf[index - j - 1] = temp;
	}
	
	outBuf[index++] = '\r';
	outBuf[index++] = '\0';
	
}



#pragma endregion private helper methods
