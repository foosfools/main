/*
 * Board.h
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

#define MY_MIN(a,b) ((a < b) ? a : b)
#define MY_MAX(a,b) ((a > b) ? a : b)

#define ROD_NUM_ELEMENTS 3

using namespace cv;
class Board
{
public:
	Board(int currentX, int currentY);
	Board();
	//updates the velocity of the ball and returns the components of the unit vector
	Vec2f updateBallVelocity();
	
	//returns a coordinate for the prediction when it hits the rod, or -1 if the ball is not going to hit within range of the rod
	Vec2i getBallPredictionOnRod(Vec2i rod[ROD_NUM_ELEMENTS]);
	double lastBallVelocity_pixPerSecX;
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
//returns last x pixVel and avg prediction position, lastXPixVel = NaN if not valid
	Vec2i avgBallOnRod(Vec2i prediction, double* lastXPixVel); 
	
	// (x_{0}, y_{0}, x_{1}, y_{1})
	Vec4i bottomBoard;
	Vec4i topBoard;
	Vec4i leftBoard;
	Vec4i rightBoard;
	/*rod1[0][i] top end of goalie rod
	rod1[1][i]   goalie location
	rod1[2][i]   bottom end of goalie rod*/
	Vec2i rod1[ROD_NUM_ELEMENTS]; 
	int convertRodtoMotorPulse(Vec2i predictionOffsetFromCurrent); 
};


#endif /* BOARD_H_ */
