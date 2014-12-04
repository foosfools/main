#include "Board.h"
#include "math.h"
#include <cassert>

Rod::Rod(int32_t xPos, int32_t minY, int32_t maxY, uint8_t numPlayers, uint8_t transMotor_num, uint8_t kickMotor_num) 
: xPos(xPos), minY(minY), maxY(maxY), numPlayers(numPlayers), transMotor_num(transMotor_num), kickMotor_num(kickMotor_num)
{
	this->currentY = (minY + maxY) / 2;
	currentAvgBallIndex = 0;
	rodSpacing_px = tableWidth -  (maxY - minY) - 2 * bushingOffset;
}


Board::Board(int currentX, int currentY)
{
	currX = currentX;
	currY = currentY;
	lastBallVelocity = 0.0;
	lastBallVelocity_pixPerSecX = 0.0;
	
	//latest components of velocity for ball
	lastXComp = 0.0;
	lastYComp = 0.0;
	
	//coordinates from previous frame of ball
	prevX = currX;
	prevY = currY;
	lastGoaliePos[0] = 0;
	lastGoaliePos[1] = 0;
	ballVelAvgArr_index = 0;
	rods[0] = new Rod(53, 
					102, 
					179, 
					1, 
					0, 
					1);
					
	rods[1] = new Rod(83, 
					43, 
					232, 
					2, 
					2, 
					3);
//int32_t xPos, int32_t minY, int32_t maxY, uint8_t numPlayers, uint8_t transMotor_num, uint8_t kickMotor_num
}



Board::Board() : Board(0, 0)
{}



Vec2f Board::updateBallVelocity(double* avgXVel)
{
	enum
	{
		maxFrame_count = 3,
	};
	
	static int frameCount = 0;
	static bool timerStarted = false;
	static clock_t startTime;
	static clock_t endTime;
	
	if(frameCount == maxFrame_count)
	{	
		frameCount = 0;
		int deltaY = currY - prevY;
		int deltaX = currX - prevX;
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
		
		if(timerStarted)
		{
			endTime = clock();
			
			double seconds = (double) (endTime - startTime) / ((double)CLOCKS_PER_SEC);
			lastBallVelocity_pixPerSecX = ( (double)deltaX ) / seconds;
		}
		
		startTime = clock();
		timerStarted = true;
	}
	
	*avgXVel = updateBallVelocityAverage(lastBallVelocity_pixPerSecX);
	
	frameCount++;
	
	return Vec2f(lastXComp, lastYComp);
}



Vec2i Board::getBallPredictionOnRod(Rod* rod)
{
	Vec2i result;
	
	float theta       = acos(lastXComp);
	int32_t distanceX = (currX - rod->xPos);
	int32_t distanceY = ((float)distanceX) * tan(theta);
	int32_t resultY   = (lastYComp < 0) ? (currY + distanceY) : (currY - distanceY);

	
	if( resultY < rod->minY || resultY > rod->maxY )
	{
		result[0] = -1;
		result[1] = -1;
	}
	else
	{
		result[0] = rod->xPos;
		result[1] = resultY;
	}
	
	return result;
}



Vec2i Board::avgBallOnRod(Vec2i prediction, Rod* rod)
{

	Vec2i result;
	static double velArr[FRAMES_TO_AVG_FOR_VEL_NUM];

	if( prediction[0] == -1 || prediction[1] == -1 )
	{
		rod->currentAvgBallIndex = 0;
		result[0] = -1;
		result[1] = -1;
		return result;
	}
	
	rod->avgBallOnRodArr[rod->currentAvgBallIndex++] = prediction;
	
	if(rod->currentAvgBallIndex == FRAMES_TO_AVG_FOR_VEL_NUM)
	{
		rod->currentAvgBallIndex = 0;
		
		int sumX   = 0;
		int sumY   = 0;
		
		for(int i = 0; i < FRAMES_TO_AVG_FOR_VEL_NUM; i++)
		{
			sumX += rod->avgBallOnRodArr[i][0];
			sumY += rod->avgBallOnRodArr[i][1]; 
		}
		result[0] = sumX / FRAMES_TO_AVG_FOR_VEL_NUM;
		result[1] = sumY / FRAMES_TO_AVG_FOR_VEL_NUM;
	}
	else
	{
		result[0] = -1;
		result[1] = -1;
	}
	
	return result;
}



//
int Board::convertRodtoEncoderVal(Rod* rod)
{
	enum
	{
		encoderScalar = 0x3FFF,
	};
	
	int scalar = rod->maxY - rod->minY;
	int scaledRodPos = rod->currentY - rod->minY;
	
	float ratio = 1.0f - ((float)scaledRodPos)/((float)scalar);
	
	return (int)(ratio * ((float)encoderScalar));
}



double Board::updateBallVelocityAverage(double lastVelX)
{
	static double velArr[FRAMES_TO_AVG_FOR_VEL_NUM];
	static int index   = 0;
	
	velArr[index++] = lastVelX;
	
	if(index == FRAMES_TO_AVG_FOR_VEL_NUM)
	{
		index = 0;
		
		int velSum = 0;
		
		for(int i = 0; i < FRAMES_TO_AVG_FOR_VEL_NUM; i++)
		{
			velSum += velArr[i];
		}

		return velSum / FRAMES_TO_AVG_FOR_VEL_NUM;
	}
	else
	{
		return 0.0;
	}
}



int getMinPlayerOffsetForRod(Rod* rod, int ballOnRodPos)
{
	static const float epsilon = 0.00000001;
	
	//add rodSpacing
	int rodDist_px = rod->maxY - rod->minY;
	
	float leftRodEnd_px = rod->currentY - ((float)rod->rodSpacing_px) / 2;
	float unNormalizedN = (((float)ballOnRodPos -  (float)leftRodEnd_px) / (float)rod->rodSpacing_px) 
							* (float)(rod->numPlayers - 1);
							
	int normalizedN = (int)(unNormalizedN + 0.5 + epsilon);
	int lowerRound  = (int)(unNormalizedN + epsilon);
	int higherRound = lowerRound + 1;
	
	assert(normalizedN == lowerRound || normalizedN == higherRound);
	
	float nThPlayerPos = leftRodEnd_px + (((float)normalizedN) / (float)(rod->numPlayers - 1))
										* rod->rodSpacing_px;
	
	int offset = ballOnRodPos - nThPlayerPos;
	
	//check that offset is not out of range
	if( offset + rod->currentY + rodDist_px / 2 > rod->maxY ||  offset + rod->currentY - rodDist_px / 2 < rod->maxY )
	{
		normalizedN = (normalizedN == lowerRound) ? higherRound : lowerRound;
		
		nThPlayerPos = leftRodEnd_px + (((float)normalizedN) / (float)(rod->numPlayers - 1))
										* rod->rodSpacing_px;
		offset = ballOnRodPos - nThPlayerPos;
	}

	return offset;
}



//EOF
