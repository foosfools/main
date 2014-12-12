/*
* hal_dance.h
*/
#ifndef HAL_DANCE_H
#define HAL_DANCE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "driverlib/rom_map.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
//#include "inc/tm4c123gh6pm.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

//step size config
#define M0_PIN GPIO_PIN_7
#define M1_PIN GPIO_PIN_1
#define M2_PIN GPIO_PIN_3
#define M0_PORT GPIO_PORTA_BASE
#define M1_PORT GPIO_PORTF_BASE
#define M2_PORT GPIO_PORTE_BASE

static uint32_t criticalRegionCount = 0;



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


typedef struct
{
	bool toggleEn; //toggleEn

	uint32_t remainTime_ms; 
	
	uint32_t step_pin;
	uint32_t step_port;
	
	uint32_t dir_pin;
	uint32_t dir_port;
	
	uint32_t sleep_pin;
	uint32_t sleep_port;
	
	uint32_t stepPin_state;

	bool directionBit;

	uint32_t stepWidth; //used for accel and decel
	uint32_t stepWidth_counter;
} motor_foop;

void motorsInit(motor_foop* motorArray, uint8_t totalMotors);

void UARTInit(void);

void systemInit(motor_foop* motorArray, uint8_t totalMotors);

void TimerInit(void);

void MOTOR_ENABLE(uint32_t num, uint32_t time_ms, motor_foop * motorArray, bool direction);	

void MOTOR_DISABLE(uint32_t num, motor_foop * motorArray);

int32_t stringToInt(char* c);

//only input is char*c. the rest are outputs. returns true on success
bool parsemotorData(char* c, uint8_t* motorNum, bool* direction, uint32_t* time_ms);
//EOF

enum
{
	minStepWidth = 1,
};

#endif