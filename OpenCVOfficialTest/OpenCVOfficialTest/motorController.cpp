
#include "motorController.h"



/*
 *  Constructor
 *
 *	initiates SPI
 */
motorController::motorController()
{
	fd = open("/dev/spidev1.0", O_RDWR);
	if(fd < 0) printf("spi failed to open\n");
}



/*
 *  Deconstructor
 */
motorController::~motorController()
{
	close(fd);
}




int prevAngularPosition = 0;

/**
 *	Rotates the goalie
 * m[0] = '$' // begin command
 * m[1] = 0/1 // which motor
 * m[2] = 0/1 -> ccw/cw // direction
 * m[3:4] // number of steps
 * m[5] = 'R' // run motor/ end of command
 */
void motorController::rotateGoalie(int newPosition)
{

	printf("rotateGoalie:  previous position: %d -> new position: %d\n",prevAngularPosition, newPosition);

	char m[7] = "$110dR";
	
	if(prevAngularPosition == newPosition) // return if no change in position
	{
		return;
	}
	else if(prevAngularPosition > newPosition)
	{
		m[2] = (char) 1; // direction CCW or beckward
		newPosition = prevAngularPosition - newPosition;
		prevAngularPosition -= newPosition;
	}
	else
	{
		m[2] = (char) 0; // direction CW or forward
		newPosition = newPosition - prevAngularPosition;
		prevAngularPosition += newPosition;
	}
	
	// int size is 2 bytes
	m[3] = (char) (newPosition >> 8); 	// 1 byte
	m[4] = (char) newPosition; 		// 
	

	printf("rotateGoalie: %d %d & position: %d\n",m[3],m[4], newPosition);

	
	sendMessage(m);
}




int prevLinearPosition = 0;
/**
 *	move the goalie
 * m[0] = '$' // begin command
 * m[1] = 0/1 // which motor
 * m[2] = 0/1 -> ccw/cw // direction
 * m[3:4] // number of steps
 * m[5] = 'R' // run motor/ end of command
 */
void motorController::moveGoalie(int newPosition)
{
	printf("moveGoalie:  previous position: %d -> new position: %d\n",prevLinearPosition, newPosition);
	
	char m[7] = "$010dR";
	
	if(prevLinearPosition == newPosition) // return if no change in position
	{
		return;
	}
	else if(prevLinearPosition > newPosition)
	{
		m[2] = (char) 0; // direction CCW or beckward
		newPosition = prevLinearPosition - newPosition;
		prevLinearPosition -= newPosition;
	}
	else
	{
		m[2] = (char) 1; // direction CW or forward
		newPosition = newPosition - prevLinearPosition;
		prevLinearPosition += newPosition;
	}
	
	// int size is 2 bytes
	m[3] = (char) (newPosition >> 8); 	// 1 byte
	m[4] = (char) newPosition; 		// 
	

	//printf("moveGoalie: %d %d & position: %d\n",m[3],m[4], newPosition);

	
	sendMessage(m);
}




/**
 *
 */
void motorController::sendMessage(char* message)
{
	volatile uint32_t count =0;

	while(count < 6)
	{
		write(fd,message+count,1);
		count++;
	}

}




/**
*
*/
int main()
{

	unsigned long i = 1;
	unsigned int j = 200;

	motorController moCtl;
	while(1)
	{
		if(i==500000000)
		{
			moCtl.moveGoalie(j);
			i = 0;
			j+= j*3;
		}
		else if(i == 250000000)
		{
			moCtl.rotateGoalie(j);
			i++;
			j-= j*2;
		}
		else
		{
			i++;
		}
	}


	return 0;
}

