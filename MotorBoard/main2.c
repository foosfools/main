#include "hal2.h"
#include "AS5048.h"
#include <stdio.h>
#include <string.h>

#define TOTAL_MOTORS 2

#define BUF_COUNT 32

static char buf[BUF_COUNT];

static bool writeToScreen = false;

static uint8_t bufIndex = 0;

static motor_foop motor_info[] = 
{	
	{.step_size = 0, .toggleGPIO_en = false, .step_pin = GPIO_PIN_6, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_1, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTB_BASE, .stepPin_state = GPIO_PIN_6, .slaveSel_port=GPIO_PORTB_BASE, .slaveSel_pin=GPIO_PIN_2},
	{.step_size = 0, .toggleGPIO_en = false, .step_pin = GPIO_PIN_7, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_4, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_3, .sleep_port=GPIO_PORTB_BASE, .stepPin_state = GPIO_PIN_7, .slaveSel_port=GPIO_PORTB_BASE, .slaveSel_pin=GPIO_PIN_5} 
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
		if(motor_info[i].toggleGPIO_en)
		{
			CRITICAL_START();
			{
				GPIOPinWrite(motor_info[i].step_port, 
					motor_info[i].step_pin, 
					motor_info[i].stepPin_state);
				motor_info[i].stepPin_state = ~motor_info[i].stepPin_state;
				if(motor_info[i].step_size == 0)
				{
					MOTOR_DISABLE(i, motor_info);
				}				
				motor_info[i].step_size--;
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
			uint32_t step_size;
			
			if(parsemotorData(buf, &motorNum, &direction, &step_size))
			{
				MOTOR_ENABLE(motorNum, step_size, motor_info, direction);
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
