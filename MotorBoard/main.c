#include "hal.h"
#include <stdio.h>
#include <string.h>

#define total_motors 1
static motor_foop motor_info[total_motors];


#define BUF_COUNT 32

static char buf[BUF_COUNT];

static bool writeToScreen = false;

static uint8_t bufIndex = 0;
volatile static int i = 0;
	
	
	
void
UARTIntHandler(void)
{
	uint32_t ui32Status = UARTIntStatus(UART0_BASE, false);
	
    UARTIntClear(UART0_BASE, ui32Status);

	char c = UARTCharGetNonBlocking(UART0_BASE);
	
	if((uint8_t)c != -1)
	{
		buf[bufIndex] = c;
		if(buf[bufIndex] == '\r')
		{
			writeToScreen = true;
			return;
		}
		bufIndex++;
		bufIndex %= BUF_COUNT;
	}
}



void
TIMER0A_Handler(void)
{

	static bool stateVar = false;
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	i++; 

	for(i = 0; i < total_motors; i++)
	{
		if(motor_info[i].pwm_en)
		{
			CRITICAL_START();
			{
				motor_info[i].remainTime_ms--;
				if(motor_info[i].remainTime_ms == 0)
				{
					motor_info[i].pwm_en = false; 
<<<<<<< HEAD
					motor_info[i].remainTime_ms = 3000;
					PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, stateVar);
=======
					//motor_info[i].remainTime_ms = 3000;
					PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, false);
>>>>>>> 72e0fe0762a2446fa5d0f2ea55bf127d9a036e15
					stateVar = !stateVar;
				}
			}
			CRITICAL_END();
		}
	}
}



int main(void)
{
    volatile uint32_t ui32Loop;
	systemInit();
<<<<<<< HEAD
=======
	PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, false);
	stepSizeSet(quarter);
>>>>>>> 72e0fe0762a2446fa5d0f2ea55bf127d9a036e15
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
    ui32Loop = SYSCTL_RCGC2_R;
    GPIO_PORTF_DIR_R = 0x08;
    GPIO_PORTF_DEN_R = 0x08;
<<<<<<< HEAD
	
=======
    int i; 
		
>>>>>>> 72e0fe0762a2446fa5d0f2ea55bf127d9a036e15
	for(;;)
	{
		if(writeToScreen)
		{
			char c = '\n';
			
			_write(0, buf, bufIndex + 1);	
			_write(0, &c, 1);
<<<<<<< HEAD
			bufIndex = 0;
			writeToScreen = false;
=======
		//	if(1)
			if(strcmp("forward\r", buf) == 0)
			{
			  GPIO_PORTF_DATA_R = 0X08;   
			  motor_info[0].pwm_en = true; 
			  motor_info[0].remainTime_ms = 1000;
 			  PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
	
			}
			bufIndex = 0;
			writeToScreen = false;

			for(i = 0; i< BUF_COUNT; i++)
			{
			   buf[i] = '\0'; 
			}	
>>>>>>> 72e0fe0762a2446fa5d0f2ea55bf127d9a036e15
		}
	}
	return 0;
}
