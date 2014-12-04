/*
 * Board.h
 */

#ifndef BOARD_H_
#define BOARD_H_
#include <sstream>
#include <string>
#include <iostream>
#include <stdint.h>
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

#define ROD_NUM_ELEMENTS (3)
#define NUM_RODS         (2)

#define FRAMES_TO_AVG_FOR_VEL_NUM (6)

using namespace cv;

class Rod
{
	public:
	Rod(int32_t xPos, int32_t minY, int32_t maxY, uint8_t numPlayers, uint8_t transMotor_num, uint8_t kickMotor_num);
	int32_t minY;
	int32_t maxY;
	//specifies the current middle of the rod
	int32_t currentY;
	int32_t xPos;
	int32_t rodSpacing_px;
	uint8_t numPlayers;
	uint8_t transMotor_num;
	uint8_t kickMotor_num;
	static const int tableWidth = 3453;
	static const int bushingOffset = 10;
	Vec2i avgBallOnRodArr[FRAMES_TO_AVG_FOR_VEL_NUM];
	uint32_t currentAvgBallIndex;
}; 

class Board
{
public:
	Board(int currentX, int currentY);
	Board();
	//updates the velocity of the ball and returns the components of the unit vector
	Vec2f updateBallVelocity(double* avgXVel);
	
	//returns a coordinate for the prediction when it hits the rod, or -1 if the ball is not going to hit within range of the rod
	Vec2i getBallPredictionOnRod(Rod* rod);
	int getMinPlayerOffsetForRod(Rod* rod, int ballOnRodPos);
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
	Vec2i avgBallOnRod(Vec2i prediction, Rod* rod); 
	
	// (x_{0}, y_{0}, x_{1}, y_{1})
	Vec4i bottomBoard;
	Vec4i topBoard;
	Vec4i leftBoard;
	Vec4i rightBoard;

	Rod*	rods[NUM_RODS];

	double ballVelAvgArr[FRAMES_TO_AVG_FOR_VEL_NUM];
	int ballVelAvgArr_index;
	
	//rodPos is absolute pixel coordinate of the rod
	int convertRodtoEncoderVal(Rod* rod);
	
	private:
	//returns x component of ball velocity average
	double updateBallVelocityAverage(double lastVelX);
};


#endif /* BOARD_H_ */
