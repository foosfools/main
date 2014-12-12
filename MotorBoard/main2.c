#include "foos_config.h"
#include "hal2.h"
#include "AS5048.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#pragma GCC optimize ("unroll-loops")

//in retarget.c
extern int _write(int file, char* ptr, int len);

typedef enum
{
	readEncoder_event,
	writeToScreen_event,
	
	numEvents,
} events;

static volatile bool eventArr[numEvents];

#define BUF_COUNT    32

static char buf[BUF_COUNT];

static uint8_t bufIndex = 0;


//PCB
static volatile motor_foop motor_info[] = 
{	
//m0 =  .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_6,
//m1 =  .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_4,
	{ .step_pin = GPIO_PIN_2, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_6, .dir_port=GPIO_PORTM_BASE, .sleep_pin=GPIO_PIN_7, .sleep_port=GPIO_PORTM_BASE, .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_4, .midPoint = 0x328E, .isKickMotor = false, .directionBit = true, .spi = spi0}, //motor0 drv6
	{ .step_pin = GPIO_PIN_1, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_5, .dir_port=GPIO_PORTP_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTB_BASE, .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_6, .midPoint = 0xCF9, .isKickMotor = true,  .directionBit = true, .spi = spi0},   //motor1 drv7
//	{ .step_pin = GPIO_PIN_0, .step_port=GPIO_PORTG_BASE, .dir_pin=GPIO_PIN_7, .dir_port=GPIO_PORTA_BASE, .sleep_pin=GPIO_PIN_3, .sleep_port=GPIO_PORTM_BASE, .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_6, .midPoint = 0x2B38, .isKickMotor = false,  .directionBit = false, .spi = spi0}, //motor2 drv3
//	{ .step_pin = GPIO_PIN_0, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_1, .dir_port=GPIO_PORTE_BASE, .sleep_pin=GPIO_PIN_0, .sleep_port=GPIO_PORTE_BASE, .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_4, .midPoint = 0x3101,  .isKickMotor = true,  .directionBit = true, .spi = spi0}  //motor3 drv8

};	


//BREADBOARD
/*
static volatile motor_foop motor_info[] = 
{	
//m0 =  .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_6,
//m1 =  .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_4,
	{ .step_pin = GPIO_PIN_3, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_4, .dir_port=GPIO_PORTM_BASE, .sleep_pin=GPIO_PIN_5, .sleep_port=GPIO_PORTM_BASE, .slaveSel_port=GPIO_PORTK_BASE, .slaveSel_pin=GPIO_PIN_7, .midPoint = 0x3A8A, .isKickMotor = false, .directionBit = true, .spi = spi0}, //motor0
	{ .step_pin = GPIO_PIN_4, .step_port=GPIO_PORTK_BASE, .dir_pin=GPIO_PIN_3, .dir_port=GPIO_PORTN_BASE, .sleep_pin=GPIO_PIN_4, .sleep_port=GPIO_PORTQ_BASE, .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_4, .midPoint = 0xCF9,  .isKickMotor = true,  .directionBit = true, .spi = spi0}  //motor1
//	{ .step_pin = GPIO_PIN_2, .step_port=GPIO_PORTF_BASE, .dir_pin=GPIO_PIN_6, .dir_port=GPIO_PORTM_BASE, .sleep_pin=GPIO_PIN_7, .sleep_port=GPIO_PORTM_BASE, .slaveSel_port=GPIO_PORTD_BASE, .slaveSel_pin=GPIO_PIN_5, .midPoint = 0x250E, .isKickMotor = false, .directionBit = true, .spi = spi1}, //motor2
//	{ .step_pin = GPIO_PIN_5, .step_port=GPIO_PORTK_BASE, .dir_pin=GPIO_PIN_1, .dir_port=GPIO_PORTN_BASE, .sleep_pin=GPIO_PIN_2, .sleep_port=GPIO_PORTN_BASE, .slaveSel_port=GPIO_PORTK_BASE, .slaveSel_pin=GPIO_PIN_3, .midPoint = 0xCF9, .isKickMotor = true,  .directionBit = true, .spi = spi1}   //motor3
};																																																																							// .endPos = 0x07FE, .midPoint = 0x07FE		
*/

	
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
	
	bool toggleI = false;
	
	for(i = 0; i < TOTAL_MOTORS; i++)
	{
		switch( motor_info[i].state )
		{
			case motorState_stopped: 
				break;
				
			#if ACC_ENABLE == 0
				case motorState_fullSpeed:
				case motorState_accel:
				case motorState_decel:
					toggleI = true;
					break;
			#else
				case motorState_accel:
					motor_info[i].stepWidth_counter++;
					if( motor_info[i].stepWidth_counter >=  motor_info[i].stepWidth )
					{
						toggleI = true;
						motor_info[i].stepWidth--;
						motor_info[i].stepWidth_counter = 0;
					
						if( motor_info[i].stepWidth <= minStepWidth )
						{
							 motor_info[i].state = motorState_fullSpeed;
						}
					}
					break;
			
				case motorState_decel:
					motor_info[i].stepWidth_counter++;
					if( motor_info[i].stepWidth_counter >=  motor_info[i].stepWidth )
					{
						toggleI = true;
						motor_info[i].stepWidth_counter = 0;
					
						if( motor_info[i].stepWidth < maxStepWidth )
						{
							motor_info[i].stepWidth++;
						}
					}
					break;
			#endif
			
			default:
				break;
		}

		if( toggleI )
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



static void updateMotorVals(void)
{
	enum
	{
		maxMotorSteps  = 200,
		stopThreshold  = 300,
		decelThreshold = 1000,
	};
	
	int32_t difference = 0;
	
	for(uint32_t i = 0; i < TOTAL_MOTORS; i++)
	{
		difference = distBetweenValues(motor_info[i].offset, 
										(uint32_t)motor_info[i].endPos, 
										(uint32_t)motor_info[i].encoderVal);
						
		if( difference > stopThreshold || difference < -stopThreshold )
		{
			if( ACC_ENABLE )
			{
				if( difference > -decelThreshold && difference < decelThreshold )
				{
					CRITICAL_START();
						setMotorState(motorState_decel, &motor_info[i]);
					CRITICAL_END();
				}
				else if( motor_info[i].state == motorState_stopped )
				{
					CRITICAL_START();
						setMotorState(motorState_accel, &motor_info[i]);
					CRITICAL_END();
				}
			}
			else
			{
				CRITICAL_START();
					motor_info[i].state = motorState_fullSpeed;
				CRITICAL_END();
			}
			
			MOTOR_ENABLE(i, motor_info, (difference < 0) ? false : true);
		}
		else
		{
			if( motor_info[i].isKickMotor &&  (motor_info[i].endPos != motor_info[i].midPoint) )
			{
				CRITICAL_START();
					motor_info[i].endPos = motor_info[i].midPoint;
			//	setMotorState(motorState_stopped, &motor_info[i]);
				CRITICAL_END();
			}
			else
			{
				MOTOR_DISABLE(i, motor_info);
			}
		}
	}
}



static void readEncoders(void)
{
		for(uint32_t i = 0; i < TOTAL_MOTORS; i++)
		{
			CRITICAL_START();
				motor_info[i].encoderVal = AS5048_readAngle(motor_info[i].spi, motor_info[i].slaveSel_port, motor_info[i].slaveSel_pin);
			CRITICAL_END();
			
			if( PRINT_CALIBRATE )
			{
				printHex16(motor_info[i].encoderVal);
			}
		}
		
	updateMotorVals();
}



static void handleWriteToScreenEvent(void)
{
		char c = '\n';
			_write(0, buf, bufIndex + 1);
			_write(0, &c, 1);

			uint8_t motorNum;
			bool direction;
			uint32_t endPos;
			
			if( parsemotorData(buf, &motorNum, &direction, &endPos) )
			{
				if( endPos > 14000 ) //makes sure that endpos is in reachable range, otherwise it would keep hitting the edge of the table
				{
					endPos = 14000;
				}
				else if( endPos <2000 )
				{
					endPos = 2000;
				} 
				
				if(motorNum < TOTAL_MOTORS)
				{
					CRITICAL_START();
						motor_info[motorNum].endPos = (endPos + motor_info[motorNum].offset) & maxEncoderVal;
					CRITICAL_END();
				}
			}
			
			CRITICAL_START();
				bufIndex = 0;
			CRITICAL_END();
			
			for(uint32_t i = 0; i < BUF_COUNT; i++)
			{
			   buf[i] = '\0'; 
			}	
}



int main(void)
{ 
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
