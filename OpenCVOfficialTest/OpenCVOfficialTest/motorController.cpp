
#include "motorController.h"



/*
 *  Constructor
 */
motorController::motorController()
{
	
	//unsigned char buf = '$'; //0xA1;

 
	fd = open("/dev/spidev1.0", O_RDWR);
	if(fd < 0) printf("spi failed to open\n");

	char *message = "$000dR";
	sendMessage(message);

	message = "$010dR";
	sendMessage(message);
}



/*
 *  Deconstructor
 */
motorController::~motorController()
{
	close(fd);
}


/**
 *
 */
void motorController::rotateGoalie(int degree)
{
	char m[7] = "$000dR";
	//char *message = "$010dR";
	//*(message + 4)= 50;//(char)(steps & 0xff);
	//
	m[4] = (char) degree;
	if(degree < 0)
	{
	m[2] = '1'; 
	}
	sendMessage(m);
}

/**
 *
 */
void motorController::moveGoalie(unsigned int steps)
{
	char m[7] = "$010dR";
	//char *message = "$010dR";
	//*(message + 4)= 50;//(char)(steps & 0xff);
	//
	m[4] = (char) steps;
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
/*
int main(){

unsigned long i = 0;
unsigned int j = 0;

motorController moCtl;
while(1)
{
	if(i==100000000)
	{
		moCtl.moveGoalie(j);
		i = 0;
		j+= 1;
	}
	else if(i == 50000000)
	{
		moCtl.rotateGoalie(j);
		i++;
		j+= 1;
	}
	else
	{
		i++;
	}
}


return 0;

}*/

