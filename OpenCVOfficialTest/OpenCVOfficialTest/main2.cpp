#include <unistd.h>
#include "DancingMotors.h"


static void danceA(DancingMotors* dance_test)
{
	enum
	{
		sleepInterval = 1000000,
		leftEdgePulse = -150,
		rightEdgePulse = 150,
	};
	for( ;; )
	{
		dance_test->writeToMotor(0, leftEdgePulse);
		dance_test->writeToMotor(2, rightEdgePulse);
		dance_test->writeToMotor(4, leftEdgePulse / 4);
		usleep(sleepInterval);
		dance_test->writeToMotor(0, rightEdgePulse * 2);
		dance_test->writeToMotor(2, leftEdgePulse * 2);
		dance_test->writeToMotor(4, rightEdgePulse / 2);
		
		dance_test->writeToMotor(1, rightEdgePulse * 12);
		dance_test->writeToMotor(3, leftEdgePulse * 6);
		dance_test->writeToMotor(5, leftEdgePulse * 3);
		usleep(sleepInterval * 2);
		
		dance_test->writeToMotor(0, leftEdgePulse);
		dance_test->writeToMotor(2, rightEdgePulse);
		dance_test->writeToMotor(4, leftEdgePulse / 4);
		usleep(sleepInterval);
	}
}


int main(int argc, char* argv[])
{
	DancingMotors* dance_test = new DancingMotors();
	
	cout << "Initialized" << endl;

	enum{
		leftEdgePulse = -500,
		rightEdgePulse = 500, 
		totalMotor     = 1, 
		sleepScale     = 2000
	}; 
	
	int currentPos[] = {0};
	
	int motorPulse = 0; 

	danceA(dance_test);

	
	return 0;
}
