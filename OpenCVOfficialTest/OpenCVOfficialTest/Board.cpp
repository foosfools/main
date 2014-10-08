#include "Board.h"
#include "math.h"


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
	
	rod1[0][0] = 53; //x component has to be the same for goalie rod
	rod1[0][1] = 102; 
	rod1[2][0] = rod1[0][0]; 
	rod1[2][1] = 179; 
	rod1[1][0] = (rod1[0][0] + rod1[2][0]) / 2; 
	rod1[1][1] = (rod1[0][1] + rod1[2][1]) / 2; 
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
	rod1[0][0] = 53; //x component has to be the same for goalie rod
	rod1[0][1] = 102; 
	rod1[2][0] = rod1[0][0]; 
	rod1[2][1] = 179; 
	rod1[1][0] = (rod1[0][0] + rod1[2][0]) / 2; 
	rod1[1][1] = (rod1[0][1] + rod1[2][1]) / 2; 
}



Vec2f Board::updateBallVelocity()
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
	
	frameCount++;
	
	return Vec2f(lastXComp, lastYComp);
}



Vec2i Board::getBallPredictionOnRod(Vec2i rod[ROD_NUM_ELEMENTS])
{
	Vec2i result;
	
	float theta = acos(lastXComp);
	
	int32_t distanceX = (currX - rod[0][0]);
	
	int32_t distanceY = ((float)distanceX) * tan(theta);
	
	int32_t resultY = (lastYComp < 0) ? (currY + distanceY) : (currY - distanceY);

	
	if( resultY < (MY_MIN(rod[0][1], rod[2][1])) || resultY > (MY_MAX(rod[0][1], rod[2][1])) )
	{
		result[0] = -1;
		result[1] = -1;
	}
	else
	{
		result[0] = rod[0][0];
		result[1] = resultY;
	}
	
	return result;
}



Vec2i Board::avgBallOnRod(Vec2i prediction, double* lastXPixVel)
{
	enum
	{
		numOfFrames = 6,
	};
	
	Vec2i result;
	static Vec2i avgArr[numOfFrames];
	static double velArr[numOfFrames];
	
	static int index = 0;
	
	if( prediction[0] == -1 || prediction[1] == -1 )
	{
		index = 0;
		result[0] = -1;
		result[1] = -1;
		*lastXPixVel = 0.0;
		return result;
	}
	velArr[index]   = lastBallVelocity_pixPerSecX;
	avgArr[index++] = prediction;
	
	if(index == numOfFrames)
	{
		index = 0;
		
		int sumX = 0;
		int sumY = 0;
		int velSum = 0;
		
		for(int i = 0; i < numOfFrames; i++)
		{
			sumX += avgArr[i][0];
			sumY += avgArr[i][1]; 
			velSum += velArr[i];
		}
		result[0] = sumX / numOfFrames;
		result[1] = sumY / numOfFrames;
		*lastXPixVel = velSum / numOfFrames;
	}
	else
	{
		result[0] = -1;
		result[1] = -1;
		*lastXPixVel = 0.0;
	}
	
	return result;
}



//
int Board::convertRodtoEncoderVal(Vec2i rodPos)
{
	enum
	{
		encoderScalar = 0x3FFF,
	};
	
	int max_rod = MY_MAX(rod1[0][1], rod1[2][1]); 
	int min_rod = MY_MIN(rod1[0][1], rod1[2][1]);
	int scalar = max_rod - min_rod;
	int scaledRodPos = rodPos[1] - min_rod;
	
	float ratio = ((float)scaledRodPos)/((float)scalar);
	
	return (int)(ratio * ((float)encoderScalar));
}



int Board::convertRodtoMotorPulse(Vec2i predictionOffsetFromCurrent)
{
	enum
	{
		motor_scalar = 675,
	};
	
	int max_rod = MY_MAX(rod1[0][1], rod1[2][1]); 
	int min_rod = MY_MIN(rod1[0][1], rod1[2][1]);
	
	float ratio = ((float)predictionOffsetFromCurrent[1])/((float)(max_rod - min_rod));
	float f_result = ratio * (2 * (motor_scalar));
	return ( (int)f_result ); 
}

//EOF
