#include "Board.h"
#include "math.h"

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
	lastGoaliePos[0] = 0;
	lastGoaliePos[1] = 0;
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
	lastGoaliePos[0] = 0;
	lastGoaliePos[1] = 0;
}



Vec2f Board::updateBallVelocity(int newX, int newY)
{
	int deltaY = newY - currY;
	int deltaX = newX - currX;
	//vector magnitude of velocity
	lastBallVelocity = sqrt((deltaX)*(deltaX) + (deltaY)*(deltaY));
	//components of velocity
	if(lastBallVelocity != 0)
	{
	lastXComp = deltaX/lastBallVelocity;
	lastYComp = deltaY/lastBallVelocity;
	}
	else
	{
		lastXComp = 0.0;
		lastYComp = 0.0;
	}
	//update components
	prevY = currY;
	prevX = currX;
	currX = newX;
	currY = newY;

	return Vec2f(lastXComp, lastYComp);
}
