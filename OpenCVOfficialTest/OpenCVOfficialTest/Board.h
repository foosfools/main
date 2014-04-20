/*
 * Board.h
 *
 *  Created on: Apr 11, 2014
 *      Author: nathanpatterson
 */

#ifndef BOARD_H_
#define BOARD_H_
#include <sstream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <ctime>

using namespace cv;
class Board
{
public:
	Board(int currentX, int currentY);
	Board();
	//updates the velocity of the ball and returns the components of the unit vector
	Vec2f updateBallVelocity();
	float lastBallVelocity;
	//latest components of velocity for ball
	float lastXComp;
	float lastYComp;
	//coordinates from previous frame of ball
	int prevX;
	int prevY;
	//coordinates from current frame of ball
	int currX;
	int currY;

	Vec2i lastGoaliePos;

	// (x_{0}, y_{0}, x_{1}, y_{1})
	Vec4i bottomBoard;
	Vec4i topBoard;
	Vec4i leftBoard;
	Vec4i rightBoard;
};


#endif /* BOARD_H_ */
