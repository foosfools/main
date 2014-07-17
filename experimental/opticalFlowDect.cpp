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

#define FRAME_WIDTH  320
#define FRAME_HEIGHT 240
#define FRAMES_PER_S 100
#define MAX_COUNT    2
#define N_FRAMES     5

static Mat curr; 
static uint32_t frameCounter = 0;
static vector<Point2f> points[2]; // points for optical flow

static const int minArr[3] = {12, 0, 117};
static const int maxArr[3] = {48, 255, 255};
 

static void clearPoints(void)
{
	while( points[0].size() )
	{
		points[0].pop_back();
	}
	while( points[1].size() )
	{
		points[1].pop_back();
	}
}


static Vec2i setBallCoord(const Mat * frame)
{
	static Mat tempFrame, grayImg, tempGray;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	static uint32_t x = 0;
	static uint32_t y = 0;
	static Vec2i result;
	//result[0] = 0;
	//result[1] = 0;
	uint32_t areaToMaximize1;


	blur(*frame, tempFrame, Size(2, 2));

	inRange(tempFrame, Scalar(minArr[0], minArr[1], minArr[2]), Scalar(maxArr[0], maxArr[1], maxArr[2]), grayImg);
	
	grayImg.copyTo(tempGray);
	areaToMaximize1 = 0;

	findContours(tempGray, contours, hierarchy, CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);
	//need this to prevent crash
	if (!hierarchy.size())
		return result;

	for (int32_t i = 0; i >= 0; i = hierarchy[i][0]) {

		Moments moment = moments((cv::Mat) contours[i]);
		double area = moment.m00;
		//maximization loop
		if (area > areaToMaximize1) {
			areaToMaximize1 = area;
			x = moment.m10 / area;
			y = moment.m01 / area;
		}
	}
	result[0] = x;
	result[1] = y;

	return result;
}



static void opticalFlow(Mat * frame, Vec2i * coords, bool needToInit) {
	//terminates over 20 iterations or when ep is < .03
	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);

	static Mat prev, colorImg;
	static vector<uchar> status;
	static vector<float> err;

	Size winSize(10, 10);

	cvtColor(*frame, curr, CV_BGR2GRAY);

	if (needToInit) {
		points[0].push_back(Point2f((float)(*coords)[0], (float) (*coords)[1]));
		points[1].push_back(Point2f((float) (*coords)[0], (float) (*coords)[1]));
		curr.copyTo(prev);
		//goodFeaturesToTrack(curr, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
		//cornerSubPix(curr, points[0], winSize, Size(-1, -1), termcrit);
		
		return;
	}
	calcOpticalFlowPyrLK(prev, curr, points[0], points[1], status, err,
			winSize, 3, termcrit, 0);

	coords[0] = (int)points[1][0].x;
	coords[1] = (int)points[1][0].y;
//	for(int i = 0; i < points[0].size(); i++)
	std::swap(points[1], points[0]);
	cv::swap(prev, curr);
}



int main()
{
	VideoCapture capture;
	Vec2i coords; // coords of ball
	Mat frame;  
	string windowName = "Bob";
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	bool needToInit = true; 
	
	namedWindow(windowName ,CV_WINDOW_AUTOSIZE);

	for(; ;)
	{
		capture >> frame;

		if( frame.empty() )
			continue;
		
		if( frameCounter++ % N_FRAMES  == 0 )
		{
			needToInit   = true; 
			clearPoints();
			coords       = setBallCoord(&frame); 
		}
		else 
		{
			needToInit = false; 
		}
		
		opticalFlow(&frame, &coords, needToInit);
		circle(frame, *(new Point(coords)), 20, Scalar(0, 0, 255), -1, 8);
		imshow(windowName, frame);	



		waitKey(1000 / FRAMES_PER_S);
	}
}

