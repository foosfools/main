/*
* hal2.h
*/
#ifndef HAL_H
#define HAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "driverlib/rom_map.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include <string.h>
//#include "inc/tm4c123gh6pm.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"



#define PRINT_CALIBRATE (1)



static volatile uint32_t criticalRegionCount = 0;



enum
{
	maxEncoderVal = 0x3FFF,
};


#define CRITICAL_START()    \
{ 							\
	IntMasterDisable();     \
	criticalRegionCount++;  \
}



#define CRITICAL_END()               \
{						    		 \
	if(--criticalRegionCount == 0)   \
		IntMasterEnable();			 \
}


	
typedef enum
{
	full,
	half,
	quarter,
	eighth,
	sixteenth,
	thirtySecond,
} stepSize;



typedef enum
{
	motorState_accel,
	motorState_decel,
	motorState_stopped,
	motorState_fullSpeed,
} motorState;



enum
{
	maxStepWidth = 6,
	minStepWidth = 1,
};



typedef struct
{
	uint32_t step_pin;
	uint32_t step_port;
	
	uint32_t dir_pin;
	uint32_t dir_port;
	
	uint32_t sleep_pin;
	uint32_t sleep_port;
	
	uint32_t stepPin_state;
	
	uint32_t slaveSel_port;
	uint32_t slaveSel_pin;
	
	uint16_t encoderVal;
	
	uint16_t endPos;
	
	uint16_t midPoint;

	int32_t offset;	//describes encoder offset from maxEncoderVal / 2 to midPoint
	
	bool isKickMotor; //determines if the motor is a rotational motor
	
	bool directionBit;
	
	motorState state;
	
	uint32_t stepWidth; //used for accel and decel
	uint32_t stepWidth_counter;
} motor_foop;



int32_t distBetweenValues(int32_t offset, uint32_t valueA, uint32_t valueB);

void UARTInit(void);

void systemInit(motor_foop* motorArray, uint8_t totalMotors);

void motorsInit(motor_foop* motorArray, uint8_t totalMotors);

void TimerInit(void);

void motorSleepDirPinsInit(motor_foop* motorArray, uint8_t totalMotors);

void MOTOR_ENABLE(uint32_t num, motor_foop * motorArray, bool direction);

void MOTOR_DISABLE(uint32_t num, motor_foop * motorArray);

int32_t stringToInt(char* c);

inline void setMotorState(motorState state, motor_foop* motor)
{
	(*motor).state = state; 
		
	if( state == motorState_accel )
	{
		(*motor).stepWidth = maxStepWidth; 
	}
	else if( state == motorState_decel )
	{
		(*motor).stepWidth = minStepWidth; 
	}
	(*motor).stepWidth_counter = 0;
}

//only input is char*c. the rest are outputs. returns true on success
bool parsemotorData(char* c, uint8_t* motorNum, bool* direction, uint32_t* time_ms);
//EOF

void printHex16(uint16_t val);

#endif