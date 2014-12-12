#include "hal_dance.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_MOTORS 6

#define BUF_COUNT 32

static char buf[BUF_COUNT];

static bool writeToScreen = false;

static uint8_t bufIndex = 0;

static motor_foop motor_info[] = 
{	
	{.remainTime_ms = 0, .toggleEn = false, .step_pin = GPIO_PIN_2, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_6, .dir_port=GPIO_PORTM_BASE, .sleep_pin=GPIO_PIN_7, .sleep_port=GPIO_PORTM_BASE},
	{.remainTime_ms = 0, .toggleEn = false, .step_pin = GPIO_PIN_1, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_5, .dir_port=GPIO_PORTP_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTB_BASE},
	{.remainTime_ms = 0, .toggleEn = false, .step_pin = GPIO_PIN_0, .step_port=GPIO_PORTG_BASE, .dir_pin=GPIO_PIN_7, .dir_port=GPIO_PORTA_BASE, .sleep_pin=GPIO_PIN_3, .sleep_port=GPIO_PORTM_BASE},
	{.remainTime_ms = 0, .toggleEn = false, .step_pin = GPIO_PIN_0, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_1, .dir_port=GPIO_PORTE_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTE_BASE},
	{.remainTime_ms = 0, .toggleEn = false, .step_pin = GPIO_PIN_4, .step_port=GPIO_PORTC_BASE, .dir_pin=GPIO_PIN_4, .dir_port=GPIO_PORTE_BASE, .sleep_pin=GPIO_PIN_5, .sleep_port=GPIO_PORTC_BASE},
	{.remainTime_ms = 0, .toggleEn = false, .step_pin = GPIO_PIN_5, .step_port=GPIO_PORTE_BASE, .dir_pin=GPIO_PIN_6, .dir_port=GPIO_PORTC_BASE, .sleep_pin=GPIO_PIN_3, .sleep_port=GPIO_PORTD_BASE}
};


	
void
UARTIntHandler(void)
{
	uint32_t ui32Status = UARTIntStatus(UART0_BASE, false);
	
    UARTIntClear(UART0_BASE, ui32Status);

	char c = UARTCharGetNonBlocking(UART0_BASE);
	
	if( (uint8_t)c != -1 )
	{
		buf[bufIndex] = c;
		if( buf[bufIndex] == '\r' )
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
		if( motor_info[i].toggleEn )
		{
			CRITICAL_START();
			{
				if( motor_info[i].remainTime_ms == 0 )
				{
					MOTOR_DISABLE(i, motor_info);
				}	
				else
				{			
					motor_info[i].remainTime_ms--;
				}
				
				GPIOPinWrite(motor_info[i].step_port,
					motor_info[i].step_pin, 
					motor_info[i].stepPin_state);
					motor_info[i].stepPin_state = ~motor_info[i].stepPin_state;
				
			}
			CRITICAL_END();
		}
	}
}



static void handleWriteToScreen(void)
{
			int i; 
			char c = '\n';
			_write(0, buf, bufIndex + 1);
			_write(0, &c, 1);

			uint8_t motorNum;
			bool direction;
			uint32_t time_ms;
			
			if( parsemotorData(buf, &motorNum, &direction, &time_ms) )
			{
				if( motorNum < TOTAL_MOTORS )
				{
					MOTOR_ENABLE(motorNum, time_ms, motor_info, direction);
				}
			}
			
			CRITICAL_START();
			{
				bufIndex = 0;
			}
			CRITICAL_END();
			
			for(i = 0; i < BUF_COUNT; i++)
			{
			   buf[i] = '\0'; 
			}
}



int main(void)
{
	systemInit(motor_info, TOTAL_MOTORS);
		
	for(;;)
	{
	
		if( writeToScreen )
		{
			writeToScreen = false;
			handleWriteToScreen();
		}
	}
	return 0;
}
