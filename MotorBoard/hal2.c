/*
* hal2.c
*/

#include "hal2.h"
#include "AS5048.h"
#include "foos_spi.h"
#include "driverlib/rom_map.h"

volatile uint32_t clockF;

void __error__(char *pcFilename, uint32_t ui32Line)
{
	_write("TEST");
}



int strcmp(const char *a,const char *b){
  if (! (*a | *b) ) return 0;
  return (*a!=*b) ? *a-*b : strcmp(++a,++b);
}



void TimerInit()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(5);
	SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(5);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
	if( PRINT_CALIBRATE )
	{
		TimerLoadSet(TIMER0_BASE, TIMER_A, clockF / 2);
	}
	else
	{
		TimerLoadSet(TIMER0_BASE, TIMER_A, clockF / 500);
	}
	
	IntMasterEnable();
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntEnable(INT_TIMER0A);
	TimerEnable(TIMER0_BASE, TIMER_A);
}



void UARTInit()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, clockF,9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
	  IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}


// 9600,
void systemInit(motor_foop* motorArray, uint8_t totalMotors)
{
	//SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
	//				 SYSCTL_XTAL_25MHZ);
	clockF = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
    				120000000);
	UARTInit();
	TimerInit();
	motorsInit(motorArray, totalMotors);
	AS5048_init(motorArray, totalMotors);
}



void stepSizePinsInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(M0_PORT, M0_PIN);
	GPIOPinTypeGPIOOutput(M1_PORT, M1_PIN);
	GPIOPinTypeGPIOOutput(M2_PORT, M2_PIN);
}



void stepSizeSet(stepSize size)
{
	switch(size)
	{
		case full:
			GPIOPinWrite(M0_PORT, M0_PIN, ~M0_PIN);
			GPIOPinWrite(M1_PORT, M1_PIN, ~M1_PIN);
			GPIOPinWrite(M2_PORT, M2_PIN, ~M2_PIN);
			break;
		
		case half:
			GPIOPinWrite(M0_PORT, M0_PIN, M0_PIN);
			GPIOPinWrite(M1_PORT, M1_PIN, ~M1_PIN);
			GPIOPinWrite(M2_PORT, M2_PIN, ~M2_PIN);
			break;
		
		case quarter:
			GPIOPinWrite(M0_PORT, M0_PIN, ~M0_PIN);
			GPIOPinWrite(M1_PORT, M1_PIN, M1_PIN);
			GPIOPinWrite(M2_PORT, M2_PIN, ~M2_PIN);
			break;
		
		case eighth:
			GPIOPinWrite(M0_PORT, M0_PIN, M0_PIN);
			GPIOPinWrite(M1_PORT, M1_PIN, M1_PIN);
			GPIOPinWrite(M2_PORT, M2_PIN, ~M2_PIN);
			break;
		
		case sixteenth:
			GPIOPinWrite(M0_PORT, M0_PIN, ~M0_PIN);
			GPIOPinWrite(M1_PORT, M1_PIN, ~M1_PIN);
			GPIOPinWrite(M2_PORT, M2_PIN, M2_PIN);
			break;
		
		case thirtySecond:
			GPIOPinWrite(M0_PORT, M0_PIN, M0_PIN);
			GPIOPinWrite(M1_PORT, M1_PIN, M1_PIN);
			GPIOPinWrite(M2_PORT, M2_PIN, M2_PIN);
			break;
			
		default:
			break;
	}
}



void MOTOR_ENABLE(uint32_t num, motor_foop * motorArray, bool direction)												    
{
	CRITICAL_START()	
	{									 									
		GPIOPinWrite(motorArray[num].dir_port, 
					motorArray[num].dir_pin, 
					(direction == motorArray[num].directionBit) ? ~motorArray[num].dir_pin : motorArray[num].dir_pin);
		GPIOPinWrite(motorArray[num].sleep_port, motorArray[num].sleep_pin, motorArray[num].sleep_pin);
		motorArray[num].toggleGPIO_en = true;  	
	}	
	CRITICAL_END()							
}



void MOTOR_DISABLE(uint32_t num,  motor_foop * motorArray)														    
{		
	CRITICAL_START()	
	{																		
		motorArray[num].toggleGPIO_en = false;   												
		GPIOPinWrite(motorArray[num].sleep_port, motorArray[num].sleep_pin, ~(motorArray[num].sleep_pin));	
	}	
	CRITICAL_END()			     			
}



void motorsInit(motor_foop* motorArray, uint8_t totalMotors)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    
	for(int i = 0; i < totalMotors; i++)
	{
		GPIOPinTypeGPIOOutput(motorArray[i].dir_port, motorArray[i].dir_pin);
		GPIOPinWrite(motorArray[i].dir_port, motorArray[i].dir_pin, motorArray[i].dir_pin);
		
		GPIOPinTypeGPIOOutput(motorArray[i].sleep_port, motorArray[i].sleep_pin);
		GPIOPinWrite(motorArray[i].sleep_port, motorArray[i].sleep_pin, ~motorArray[i].sleep_pin);
		
		GPIOPinTypeGPIOOutput(motorArray[i].step_port, motorArray[i].step_pin);
		GPIOPinWrite(motorArray[i].step_port, motorArray[i].step_pin, ~motorArray[i].step_pin);
		
		motorArray[i].offset = (int32_t)motorArray[i].midPoint - (maxEncoderVal / 2);
	}
	
	
}


//string must be of form "motorNum:positiveOrNegativetime_ms"
bool parsemotorData(char* c, uint8_t* motorNum, bool* direction, uint32_t* time_ms)
{
	enum{
		motorNum_index = 0,
		time_ms_index = 2,
	};
	
	*motorNum = c[motorNum_index] - '0';
	
	if(c[motorNum_index + 1] != ':')
	{
		return false;
	}
	
	int32_t tempTime_ms = stringToInt(&c[time_ms_index]);
	
	if(tempTime_ms < 0)
	{
		*direction = true;
		*time_ms = -tempTime_ms;
	}
	else
	{
		*direction = false;
		*time_ms = tempTime_ms;
	}
	return true;
}



int32_t stringToInt(char* c)
{
	bool isNegative = false;
	uint8_t count = 0;
	char ch = c[0];
	
	if(ch == '-')
	{
		count++;
		isNegative = true;
	}
	
	int32_t result = 0;

	ch = c[count];
	
	while(ch != '\0')
	{		
		if(ch == '\r')
		{
			break;
		}
		result *= 10;
		result += (ch - '0');
		count++;
		ch = c[count];
	}
	
	return (isNegative) ? -result : result;
}


inline void endian16_swap(uint16_t* n)
{
	uint16_t a = (*n) << 8;
	*n = (*n) >> 8;
	*n |= a;
}


void printHex16(uint16_t val)
{
	enum
	{
		bufSize = 9,
	};
	
	char buf[bufSize];
	buf[0] = '0';
	buf[1] = 'x';
	buf[6] = '\n';
	buf[7] = '\r';
	buf[8] = '\0';
	
	for(int i = 5; i >= 2; i--)
	{
		buf[i] = ((val & 0xF) > 9) ? (((val & 0xF) - 10) + 'A') : ((val & 0xF) + '0');
		val = val >> 4;
	}
	
	_write(0, buf, bufSize);
}


int32_t distBetweenValues(int32_t offset, uint32_t valueA, uint32_t valueB)
{
	int32_t result = (((int)valueA - offset) & maxEncoderVal) - (((int)valueB - offset) & maxEncoderVal);
	
	return result;
}

//EOF