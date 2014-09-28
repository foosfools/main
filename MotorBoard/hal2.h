/*
* hal2.h
*/
#ifndef HAL_H
#define HAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include <string.h>
#include "inc/tm4c123gh6pm.h"
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
	uint32_t step_size; 
	bool toggleGPIO_en; 
	
	uint32_t step_pin;
	uint32_t step_port;
	
	uint32_t dir_pin;
	uint32_t dir_port;
	
	uint32_t sleep_pin;
	uint32_t sleep_port;
	
	uint32_t stepPin_state;
	
	uint32_t slaveSel_port;
	uint32_t slaveSel_pin;
} motor_foop;



void UARTInit(void);

void systemInit(motor_foop* motorArray, uint8_t totalMotors);

//void PWMInit(motor_foop* motorArray, uint32_t totalMotors);

void stepSizePinsInit(void);

void stepSizeSet(stepSize size);

void TimerInit(void);

void motorSleepDirPinsInit(motor_foop* motorArray, uint8_t totalMotors);

void MOTOR_ENABLE(uint32_t num, uint32_t step_size, motor_foop * motorArray, bool direction);

void MOTOR_DISABLE(uint32_t num, motor_foop * motorArray);

int32_t stringToInt(char* c);

//only input is char*c. the rest are outputs. returns true on success
bool parsemotorData(char* c, uint8_t* motorNum, bool* direction, uint32_t* time_ms);
//EOF

inline void endian16_swap(uint16_t* n);

#endif