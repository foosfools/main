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

#define FRAME_WIDTH  640
#define FRAME_HEIGHT 480
#define FRAMES_PER_S 100
#define MAX_COUNT    2
#define N_FRAMES     30 //recalibrates every n frames

#define NONZERO_SIZE(mat) (mat.size().height != 0 && mat.size().width != 0)

static Mat curr; 
static uint32_t frameCounter = 0;
static vector<Point2f> points[2]; // points for optical flow

static string windowName = "Bob";

//static const int minArr[3] = {12, 0, 117};
//static const int maxArr[3] = {48, 255, 255};
 
 
static const int minLightArr[3] = {0, 58, 156};
static const int maxLightArr[3] = {37, 109, 255};



static const int minArr[3] = {0, 30, 163};
static const int maxArr[3] = {74, 104, 203};

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


static int* setBallCoord(const Mat * frame)
{
	static Mat tempFrame, grayImg, tempGray;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	static uint32_t x = 0;
	static uint32_t y = 0;
	static int result[2];
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


void drawOptFlowBall (const Mat& flow, Mat& cflowmap, const Scalar& color, int* coords) 
{
	int x = coords[0];
	int y = coords[1];
	
	const Point2f& fxy = flow.at< Point2f>(y, x);  
	
	 coords[0] = cvRound(x+fxy.x);
	 coords[1] = cvRound(y+fxy.y);
}

void drawOptFlowMap (const Mat& flow, Mat& cflowmap, int step, const Scalar& color) {  
 for(int y = 0; y < cflowmap.rows; y += step)  
        for(int x = 0; x < cflowmap.cols; x += step)  
        {  
            const Point2f& fxy = flow.at< Point2f>(x, y);  
          //  line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),  
             //    color);  
            circle(cflowmap, Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), 1, color, -1);  
        }  
}  

static void opticalFlowFarneback(Mat * frame, bool needToInit, Mat* cflow, int* coords) {
	static Mat prev;
	static Mat flow;

	if(needToInit)
	{
		cvtColor(*frame, prev, CV_BGR2GRAY);
		return;
	}
	
	cvtColor(*frame, curr, CV_BGR2GRAY);
	calcOpticalFlowFarneback(prev, curr, flow, 0.5, 3, 15, 3, 5, 1.1, 0);

	const Point2f& fxy = flow.at< Point2f>(coords[1], coords[0]); 
	
	 coords[0] = cvRound(coords[0]+fxy.x);
	 coords[1] = cvRound(coords[1]+fxy.y);
	cv::swap(prev, curr);
}



static void opticalFlowLK(Mat * frame, int * coords, bool needToInit) {
	//terminates over 20 iterations or when ep is < .03
	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);

	static Mat prev;
	static vector<uchar> status;
	static vector<float> err;

	Size winSize(10, 10);

	cvtColor(*frame, curr, CV_BGR2GRAY);

	if (needToInit) {
		points[0].push_back(Point2f((float) coords[0], (float) coords[1]));
		points[1].push_back(Point2f((float) coords[0], (float) coords[1]));
		curr.copyTo(prev);
		//goodFeaturesToTrack(curr, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
		//cornerSubPix(curr, points[0], winSize, Size(-1, -1), termcrit);
		
		return;
	}
	calcOpticalFlowPyrLK(prev, curr, points[0], points[1], status, err,
			winSize, 3, termcrit, 0);

	coords[0] = (int)points[1][0].x;
	coords[1] = (int)points[1][0].y;
	
	cout << "c[0] = " << coords[0] << endl;
	cout << "c[1] = " << coords[1] << endl;
//	for(int i = 0; i < points[0].size(); i++)
	std::swap(points[1], points[0]);
	cv::swap(prev, curr);
}



int main()
{
	VideoCapture capture;
	int* coords; // coords of ball
	Mat frame;  
	Mat flow;
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
		
		opticalFlowFarneback(&frame, needToInit, &frame, coords);

		if(!NONZERO_SIZE(frame))
		{
			continue;
		}
			
		circle(frame, Point(coords[0], coords[1]), 5, CV_RGB(0,255,0), -1); 
		imshow(windowName, frame);

		waitKey(1000 / FRAMES_PER_S);
	}
}