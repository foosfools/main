#ifndef _DANCINGMOTORS_H
#define _DANCINGMOTORS_H

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
#include <fcntl.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include "Board.h"
#include "../../MotorBoard/virtualPort/virtualPort.h"
//#include "motorController.h"
#define TMAX 100

#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240
#define THRESHOLD 15
#define MAX_COUNT 400
#define CENTER_TO_GOAL 60.0/19.0
//number of elements to average over during initialization
#define N_ELEMENTS 10

#define GREEN_MIN 85, 197, 45
#define GREEN_MAX 152, 255, 173
using namespace cv;
using namespace std;

class DancingMotors {
public:
	DancingMotors();
	//static functions
	static void createMotorCommand(int motorPulse, int motorNum, char* outBuf);
	void writeToMotor(int motorNum, int motorStep);
	
private:
	int uart_fd;
};
#endif


