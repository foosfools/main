#include <sstream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

using namespace std;
using namespace cv;

#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240
#define FRAMES_PER_S 100
#define MAX_COUNT 400

static Mat frame; 


void opticalFlow() {
	//terminates over 20 iterations or when ep is < .03
	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);

	static Mat prev, curr, colorImg;
	static vector<Point2f> points[2];
	static vector<uchar> status;
	static vector<float> err;
	points[0].push_back(Point2f((float) FRAME_WIDTH/2, (float) FRAME_HEIGHT/2));
	points[1].push_back(Point2f((float) FRAME_WIDTH/2, (float) FRAME_HEIGHT/2));
	Size winSize(10, 10);
	static bool needToInit = true;

	cvtColor(frame, curr, CV_BGR2GRAY);

	if (needToInit) {
		curr.copyTo(prev);
		goodFeaturesToTrack(curr, points[0], MAX_COUNT, 0.01, 10, Mat(), 3,
				0, 0.04);
		cornerSubPix(curr, points[0], winSize, Size(-1, -1), termcrit);
		needToInit = false;
		return;
	}
	calcOpticalFlowPyrLK(prev, curr, points[0], points[1], status, err,
			winSize, 3, termcrit, 0);

	circle(frame, points[1][0], 20, Scalar(0, 0, 255), -1, 8);

	std::swap(points[1], points[0]);
	cv::swap(prev, curr);
}



int main()
{
	VideoCapture capture;  
	string windowName = "Bob";
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	namedWindow(windowName ,CV_WINDOW_AUTOSIZE);

	for(; ;)
	{
		capture >> frame;

		if( frame.empty() )
			continue;
		
		opticalFlow();

		imshow(windowName, frame);	

		waitKey(1000 / FRAMES_PER_S);
	}	
} 
