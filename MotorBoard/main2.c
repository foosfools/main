#include "hal2.h"
#include "AS5048.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//#define EVENT(evt) void handle_##evt()


typedef enum
{
	readEncoder_event,
	writeToScreen_event,
	
	numEvents,
} events;

static volatile bool eventArr[numEvents];

#define TOTAL_MOTORS 2

#define BUF_COUNT 32

static char buf[BUF_COUNT];

static uint8_t bufIndex = 0;





static motor_foop motor_info[] = 
{	
	{.step_size = 0, .toggleGPIO_en = false, .step_pin = GPIO_PIN_6, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_1, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTB_BASE, .stepPin_state = GPIO_PIN_6, .slaveSel_port=GPIO_PORTB_BASE, .slaveSel_pin=GPIO_PIN_2, .encoderVal = 0, .endPos = 0x2280},
	{.step_size = 0, .toggleGPIO_en = false, .step_pin = GPIO_PIN_7, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_4, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_3, .sleep_port=GPIO_PORTB_BASE, .stepPin_state = GPIO_PIN_7, .slaveSel_port=GPIO_PORTB_BASE, .slaveSel_pin=GPIO_PIN_5, .encoderVal = 0, .endPos = 0} 
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
			eventArr[writeToScreen_event] = true;
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
	
	CRITICAL_START();
	{
		eventArr[readEncoder_event] = true;
	}
	CRITICAL_END();
}



static void updateMotorVals()
{
	enum
	{
		maxEncoderVal = 0x3FFF,
		maxMotorSteps = 200,
	};
	const int32_t threshold = 150;
	
	CRITICAL_START();
	{
		int32_t difference = ((int32_t)motor_info[0].endPos - (int32_t)motor_info[0].encoderVal);
	
		if( difference > threshold || difference < -threshold )
		{
			MOTOR_ENABLE(0, ((difference) * maxMotorSteps) / maxEncoderVal, motor_info, (difference < 0) ? false : true);
		}
 	}
 	CRITICAL_END();
}


static void readEncoders()
{
	motor_info[0].encoderVal = AS5048_readAngle(motor_info[0].slaveSel_port, motor_info[0].slaveSel_pin);
//	printHex16(motor_info[0].encoderVal);
	updateMotorVals();
}


static void handleWriteToScreenEvent()
{
		char c = '\n';
			_write(0, buf, bufIndex + 1);
			_write(0, &c, 1);
			
			bufIndex = 0;

			uint8_t motorNum;
			bool direction;
			uint32_t step_size;
			
			if(parsemotorData(buf, &motorNum, &direction, &step_size))
			{
				motor_info[motorNum].endPos = step_size;
				readEncoders();
				//MOTOR_ENABLE(motorNum, step_size, motor_info, direction);
			}
			
			bufIndex = 0;

			for(uint32_t i = 0; i < BUF_COUNT; i++)
			{
			   buf[i] = '\0'; 
			}	
}

int main(void)
{
    volatile uint32_t ui32Loop;
	systemInit(motor_info, TOTAL_MOTORS);

	int i; 
		
	for(;;)
	{
		for(uint32_t event = 0; event < numEvents; event++)
		{
			if( eventArr[event] )
			{
				eventArr[event] = false;
				
				switch(event)
				{
					case readEncoder_event:
						readEncoders();
						break;
						
					case writeToScreen_event:
						handleWriteToScreenEvent();
						break;
						
					default:
						break;
				}
			}
		}
	}
	
	return 0;
}
