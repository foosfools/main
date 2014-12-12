// DancingMotors.cpp : Defines the entry point for the console application.
//

// TODO: reference additional headers your program requires here
#include <sstream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <ctime>
#include <unistd.h>

#define TMAX 100
#define MAX_COUNT 400
#define DISPLAY_WINDOWS true

#include "DancingMotors.h"
#include "Board.h"
#include "math.h"
using namespace cv;
using namespace std;

// Setup phase-
// Lighting
// Circle init

DancingMotors::DancingMotors() {
	uart_fd = virtualPort_open(); 
	configure_port(uart_fd); 
}

#pragma region


void DancingMotors::writeToMotor(int motorNum, int motorStep)
{
	const uint8_t bufSize = 16;
	char outBufA[bufSize];
	memset(outBufA, '\0', bufSize);
	createMotorCommand(motorStep, motorNum, outBufA);
	//_write(0, &outBufA, 1);
	write(uart_fd, outBufA, strlen(outBufA));
}

//"1:1000"
void DancingMotors::createMotorCommand(int motorPulse, int motorNum, char* outBuf)
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