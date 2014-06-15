#include <csignal>
#include "ff.h"
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <sstream>
#include <fstream>

using namespace cv; 
using namespace std; 

#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

#define FRAME_RATE 5

static VideoCapture capture;
static Mat frame;
static const string windowName = "Bob";
static string sampleSign; 
static ofstream fileStream;


#define NEG_IMG_STR "negative_images/negImg"
#define POS_IMG_STR "positive_images/posImg"

void processPositives()
{
	uint32_t stupidBufSize = 40;
	char stupidBuf[stupidBufSize];
	stringstream ss;

	memset(stupidBuf, '\0', stupidBufSize);
	
	static uint32_t count = 0;
	
	ss << count;

	strcat(stupidBuf, POS_IMG_STR);
	strcat(stupidBuf, ss.str().c_str());
	strcat(stupidBuf, ".jpg");
	
	string fileName = (string)stupidBuf;
	cout << fileName << endl;
	imwrite(fileName, frame);

	fileStream << fileName;
	fileStream << "\n";
	
	count++;
}



void processNegatives()
{
	uint32_t stupidBufSize = 27;
	char stupidBuf[stupidBufSize];
	stringstream ss;

	memset(stupidBuf, '\0', stupidBufSize);
	
	static uint32_t count = 0;
	
	ss << count;

	strcat(stupidBuf, NEG_IMG_STR);
	strcat(stupidBuf, ss.str().c_str());
	strcat(stupidBuf, ".jpg");
	
	string fileName = (string)stupidBuf;
	cout << fileName << endl;
	imwrite(fileName, frame);

	fileStream << fileName;
	fileStream << "\n";
	
	count++;
}


void handler(int arg)
{
	fileStream.close();
	exit(0);
}


int main()
{	
	signal(SIGINT, handler);
	cout << "Press p to create positive samples, n for negative\n\r" << endl;
	cin >> sampleSign;
	bool positive = sampleSign[0] == 'p' ? true : false;

	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	cout << "Loading...\n\r" << endl;
	
	if(positive)
	{
		fileStream.open("info.dat");
	}
	else
	{
		fileStream.open("bg.txt");	
	}

	while(1)
	{
		capture >> frame;

		if(frame.empty())
			continue;

		if(positive)
		{
			processPositives();
		}
		else
		{
			processNegatives();
		}


		namedWindow(windowName, CV_WINDOW_AUTOSIZE);
		imshow(windowName, frame);
		waitKey(1000 / FRAME_RATE);
	}	
	return 0; 
}	
