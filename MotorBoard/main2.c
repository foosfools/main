#include "hal2.h"
#include "AS5048.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#pragma GCC optimize ("unroll-loops")

typedef enum
{
	readEncoder_event,
	writeToScreen_event,
	
	numEvents,
} events;

static volatile bool eventArr[numEvents];

#define TOTAL_MOTORS 2
#define BUF_COUNT    32

static char buf[BUF_COUNT];

static uint8_t bufIndex = 0;



static volatile motor_foop motor_info[] = 
{	
	{.toggleGPIO_en = false, .step_pin = GPIO_PIN_6, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_1, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTB_BASE, .stepPin_state = GPIO_PIN_6, .slaveSel_port=GPIO_PORTB_BASE, .slaveSel_pin=GPIO_PIN_2, .encoderVal = 0, .endPos =  0x34D0, .midPoint = 0x34D0, .isKickMotor = false, .directionBit = true},
	{.toggleGPIO_en = false, .step_pin = GPIO_PIN_7, .step_port=GPIO_PORTB_BASE, .dir_pin=GPIO_PIN_4, .dir_port=GPIO_PORTB_BASE, .sleep_pin=GPIO_PIN_3, .sleep_port=GPIO_PORTB_BASE, .stepPin_state = GPIO_PIN_7, .slaveSel_port=GPIO_PORTB_BASE, .slaveSel_pin=GPIO_PIN_5, .encoderVal = 0, .endPos = 0x2F15, .midPoint = 0x2F15, .isKickMotor = true, .directionBit = true} 
};																																																																							// .endPos = 0x07FE, .midPoint = 0x07FE		


	
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



void TIMER0A_Handler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	uint32_t i; 
	static bool readEncoder = true;
	
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
			}
			CRITICAL_END();
		}
	}

	CRITICAL_START();
	if( readEncoder )
	{
		eventArr[readEncoder_event] = true;
	}
	CRITICAL_END();
	readEncoder = !readEncoder;
}



static void updateMotorVals()
{
	enum
	{
		maxMotorSteps = 200,
		threshold     = 100,
	};
	
	CRITICAL_START();
	{
		volatile int32_t difference = 0;
		
		for(uint32_t i = 0; i < TOTAL_MOTORS; i++)
		{
			//difference = ((int32_t)motor_info[i].endPos - (int32_t)motor_info[i].encoderVal);
			difference = distBetweenValues(motor_info[i].offset, (uint32_t)motor_info[i].endPos, (uint32_t)motor_info[i].encoderVal);
			if( difference > threshold || difference < -threshold )
			{
				MOTOR_ENABLE(i, motor_info, (difference < 0) ? false : true);
			}
			else
			{
			
				if( motor_info[i].isKickMotor &&  (motor_info[i].endPos != motor_info[i].midPoint) )
				{
					motor_info[i].endPos = motor_info[i].midPoint;
				}
				else
				{
					MOTOR_DISABLE(i, motor_info);
				}
			}
		}
 	}
 	CRITICAL_END();
}


static void readEncoders()
{
	for(uint32_t i = 0; i < TOTAL_MOTORS; i++)
	{
		motor_info[i].encoderVal = AS5048_readAngle(motor_info[i].slaveSel_port, motor_info[i].slaveSel_pin);
		
		if( PRINT_CALIBRATE )
		{
			printHex16(motor_info[i].encoderVal);
		}
	}
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
			uint32_t endPos;
			
			if(parsemotorData(buf, &motorNum, &direction, &endPos))
			{
				if( endPos > 15500 ) //makes sure that endpos is in reachable range, otherwise it would keep hitting the edge of the table
				{
					endPos = 15500;
				}
				else if( endPos < 900 )
				{
					endPos = 900;
				} 
				
				motor_info[motorNum].endPos = (endPos + motor_info[motorNum].offset) & maxEncoderVal;
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
