#include <sstream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

using namespace cv;
using namespace std;

static VideoCapture capture;
static const string windowName          = "Kalman Window Yo!!!";
static const string windowNameGray      = "Gray yo!!!";
static KalmanFilter KF;

static int minArr1[3] = {0, 20, 63};
static int maxArr1[3] = {33, 100, 255};

static void kalmanFilterInit()
{
/*
	KF = KalmanFilter(6,2,0);
	
	KF.transitionMatrix = *(Mat_<float>(6, 6) << 1,0,1,0,0.5,0, 0,1,0,1,0,0.5, 0,0,1,0,1,0, 0,0,0,1,0,1, 0,0,0,0,1,0, 0,0,0,0,0,1);
	KF.measurementMatrix = *(Mat_<float>(2, 6) << 1,0,1,0,0.5,0, 0,1,0,1,0,0.5);
 */
 	KF = KalmanFilter(4,2,0);

	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
	KF.measurementMatrix = *(Mat_<float>(2, 4) << 1,0,1,0, 0,1,0,1);
	// init...
	KF.statePre.at<float>(0) = 0;//mouse_info.x;
	KF.statePre.at<float>(1) = 0;//mouse_info.y;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(KF.errorCovPost, Scalar::all(.1));
}



static Point getKalmanPoint(Point measuredPoint)
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



//outputs gray image
static Mat calibrate(Mat frame1, int * minArr, int * maxArr)
{
	Mat grayImg;
	static bool isPrinted = false;

	blur(frame1, frame1, Size(2, 2));
	inRange(frame1, Scalar(minArr[0], minArr[1], minArr[2]), Scalar(maxArr[0], maxArr[1], maxArr[2]),
			grayImg);

	return grayImg;
}



static void findColoredObject(Mat &grayImg, int &x, int &y) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;
	grayImg.copyTo(temp);
	uint32_t areaToMaximize1 = 0;

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


void drawObject(Mat &frame, int x, int y, double blue,
		double green, double red) {
	if (x > FRAME_WIDTH || y > FRAME_HEIGHT)
		return;

	Point center(x, y);
	circle(frame, center, 20, Scalar(blue, green, red), -1, 8);
}


int main(void)
{
	Mat frame1;
	Mat grayFrame;
	
	int x,y;
	Point colorP, kalmanP;
	kalmanFilterInit();
	
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	
	while (1) 
	{
		capture >> frame1;

		if (frame1.empty())
			continue;
			
		grayFrame = calibrate(frame1, minArr1, maxArr1);
		findColoredObject(grayFrame, x, y);
		colorP.x = x;
		colorP.y = y;
		
		kalmanP = getKalmanPoint(colorP);
		
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		namedWindow(windowNameGray, CV_WINDOW_AUTOSIZE);
		drawObject(frame1, kalmanP.x, kalmanP.y, 0, .9, 0);
		drawObject(frame1, colorP.x, colorP.y, 0, 255.0, 0);
		imshow(windowName, frame1);
		imshow(windowNameGray, grayFrame);
		waitKey(10);
	}

	
	return 0;
}








