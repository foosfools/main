#include <iostream>
#include <string.h>
#include <limits.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#define FRAME_WIDTH  320
#define FRAME_HEIGHT 240

using namespace std;
using namespace cv;

static const string windowName     = "Calibrate Window Bitch!!!";
static const string windowNameGray = "Output BITCH!!!";

static int minArr[3] = {12, 0, 117};
static int maxArr[3] = {48, 255, 255};
 
//static int minArr[3] = { 0, 0, 0};
//static int maxArr[3] = { UINT8_MAX, UINT8_MAX, UINT8_MAX };

static VideoCapture capture;



void on_trackbar(int, void*) {
	// trackbar position is changed
}



static void createTrackbars() {
	//create window for trackbars
	string trackbarWindowName = "Track Bars";
	//createButton("Track",TrackCallBack, this, CV_PUSH_BUTTON, 0);
	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "minArr[2]", minArr[2]);
	sprintf(TrackbarName, "maxArr[2]", maxArr[2]);
	sprintf(TrackbarName, "minArr[0]", minArr[0]);
	sprintf(TrackbarName, "maxArr[0]", maxArr[0]);
	sprintf(TrackbarName, "minArr[1]", minArr[1]);
	sprintf(TrackbarName, "maxArr[1]", maxArr[1]);
	createTrackbar("minArr[2]", trackbarWindowName, &minArr[2], maxArr[2], on_trackbar);
	createTrackbar("maxArr[2]", trackbarWindowName, &maxArr[2], maxArr[2], on_trackbar);
	createTrackbar("minArr[0]", trackbarWindowName, &minArr[0], maxArr[0], on_trackbar);
	createTrackbar("maxArr[0]", trackbarWindowName, &maxArr[0], maxArr[0], on_trackbar);
	createTrackbar("minArr[1]", trackbarWindowName, &minArr[1], maxArr[1], on_trackbar);
	createTrackbar("maxArr[1]", trackbarWindowName, &maxArr[1], maxArr[1], on_trackbar);
}



static void calibrate(void)
{
	Mat grayImg, frame1;
	int x = 0;
	int y = 0;
	static bool isPrinted = false;
	while (1) {
		capture >> frame1;

		if (frame1.empty())
			continue;

		blur(frame1, frame1, Size(2, 2));

		inRange(frame1, Scalar(minArr[0], minArr[1], minArr[2]), Scalar(maxArr[0], maxArr[1], maxArr[2]),
				grayImg);
				
		cout << "static const int minArr[3] = {" << minArr[0] << ", " << minArr[1] << ", " << minArr[2] << "};" <<endl;
		cout << "static const int maxArr[3] = {" << maxArr[0] << ", " << maxArr[1] << ", " << maxArr[2] << "};" <<endl;
		//erode(grayImg, grayImg, element);
		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		namedWindow(windowNameGray, CV_WINDOW_AUTOSIZE);
		imshow(windowName, frame1);
		imshow(windowNameGray, grayImg);

		waitKey(10);
	}
}



int main(void)
{
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	
	createTrackbars();
	calibrate();
	
	return 0;
}