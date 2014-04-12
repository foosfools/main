#include "Board.h"


Board::Board(int currentX, int currentY)
{
	currX = currentX;
	currY = currentY;
	lastBallVelocity = 0.0;
	//latest components of velocity for ball
	lastXComp = 0.0;
	lastYComp = 0.0;
	//coordinates from previous frame of ball
	prevX = currX;
	prevY = currY;
}

Board::Board()
{
	currX = 0;
	currY = 0;
	lastBallVelocity = 0.0;
	//latest components of velocity for ball
	lastXComp = 0.0;
	lastYComp = 0.0;
	//coordinates from previous frame of ball
	prevX = currX;
	prevY = currY;
}

