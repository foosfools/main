/*
* hal.h
*/
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

#define CRITICAL_START()  \
{ 												\
	IntMasterDisable();     \
	criticalRegionCount++;  \
}

#define CRITICAL_END()           \
{											       		 \
	if(--criticalRegionCount == 0) \
		IntMasterEnable();				   \
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
	uint32_t remainTime_ms; 
	bool pwm_en; 
	
} motor_foop;




typedef enum
{
	motor0_pin = GPIO_PIN_6,
} motor_pin;

typedef enum
{
	motor0_port = GPIO_PORTB_BASE,
} motor_port;

void UARTInit(void);

void systemInit(void);

void PWMInit(void);

void stepSizePinsInit(void);

void stepSizeSet(stepSize size);

void TimerInit(void);

//EOF
