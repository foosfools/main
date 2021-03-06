#include "hal.h"
#include <stdio.h>
#include <string.h>

#define TOTAL_MOTORS 2

#define BUF_COUNT 32

static char buf[BUF_COUNT];

static bool writeToScreen = false;

static uint8_t bufIndex = 0;

static motor_foop motor_info[] = 
{	
	{.remainTime_ms = 0, .pwm_en = false, .step_pin = GPIO_PIN_6, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_1, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTB_BASE, .pwm_base = PWM0_BASE, .pwm_outBit = PWM_OUT_0_BIT},
	{.remainTime_ms = 0, .pwm_en = false, .step_pin = GPIO_PIN_7, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_4, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_3, .sleep_port=GPIO_PORTB_BASE, .pwm_base = PWM0_BASE, .pwm_outBit = PWM_OUT_1_BIT}
};


	
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
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	uint32_t i; 

	for(i = 0; i < TOTAL_MOTORS; i++)
	{
		if(motor_info[i].pwm_en)
		{
			CRITICAL_START();
			{
				if(motor_info[i].remainTime_ms == 0)
				{
					MOTOR_DISABLE(i, motor_info);
				}				
				motor_info[i].remainTime_ms--;
			}
			CRITICAL_END();
		}
	}
}



int main(void)
{
    volatile uint32_t ui32Loop;
	systemInit(motor_info, TOTAL_MOTORS);

	int i; 
		
	for(;;)
	{
		if(writeToScreen)
		{
			char c = '\n';
			_write(0, buf, bufIndex + 1);
			_write(0, &c, 1);
			
			bufIndex = 0;
			writeToScreen = false;

			uint8_t motorNum;
			bool direction;
			uint32_t time_ms;
			
			if(parsemotorData(buf, &motorNum, &direction, &time_ms))
			{
				if(!motor_info[motorNum].pwm_en)
				MOTOR_ENABLE(motorNum, time_ms, motor_info, direction);
			}
			
			bufIndex = 0;
			writeToScreen = false;

			for(i = 0; i < BUF_COUNT; i++)
			{
			   buf[i] = '\0'; 
			}	
		}
	}
	return 0;
}
