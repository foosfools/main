/*
 * Board.h
 *
 *  Created on: Apr 11, 2014
 *      Author: nathanpatterson
 */

#ifndef BOARD_H_
#define BOARD_H_


class Board
{
public:
	Board(int currentX, int currentY);
	Board();
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
};


#endif /* BOARD_H_ */
