/*
* hal_dance.c
*/

#include "hal_dance.h"
#include "driverlib/rom_map.h"

volatile uint32_t clockF;

int strcmp(const char *a,const char *b){
  if (! (*a | *b)) return 0;
  return (*a!=*b) ? *a-*b : strcmp(++a,++b);
}
 
void TimerInit()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(5);
	SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(5);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, clockF / 400);
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

    UARTConfigSetExpClk(UART0_BASE, clockF, 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
  IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}



void systemInit(motor_foop* motorArray, uint8_t totalMotors)
{
	clockF = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
    				120000000);
	//SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	UARTInit();
	TimerInit();
	motorsInit(motorArray, totalMotors);
	
}



void MOTOR_ENABLE(uint32_t num, uint32_t time_ms, motor_foop * motorArray, bool direction)												    
{
	CRITICAL_START()	
	{									 									
		GPIOPinWrite(motorArray[num].dir_port, 
					motorArray[num].dir_pin, 
					(direction == motorArray[num].directionBit) ? ~motorArray[num].dir_pin : motorArray[num].dir_pin);
		GPIOPinWrite(motorArray[num].sleep_port, motorArray[num].sleep_pin, motorArray[num].sleep_pin);
		motorArray[num].remainTime_ms = time_ms; //motor_info
		motorArray[num].toggleEn = true; //motor_info
	}	
	CRITICAL_END()							
}



void MOTOR_DISABLE(uint32_t num,  motor_foop * motorArray)														    
{		
	CRITICAL_START()	
	{	
		GPIOPinWrite(motorArray[num].sleep_port, motorArray[num].sleep_pin, ~(motorArray[num].sleep_pin));
		motorArray[num].remainTime_ms = 0;	//motor_info
		motorArray[num].toggleEn = false; //motor_info					
	}	
	CRITICAL_END()			     			
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
	bool isNegative;
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

void motorsInit(motor_foop* motorArray, uint8_t totalMotors)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
		
	for(int i = 0; i < totalMotors; i++)
	{
		motorArray[i].stepWidth_counter = 0;
		motorArray[i].stepWidth         = minStepWidth;
		motorArray[i].stepPin_state     = motorArray[i].step_pin;
		motorArray[i].toggleEn          = false;
		
		GPIOPinTypeGPIOOutput(motorArray[i].dir_port, motorArray[i].dir_pin);
		GPIOPinWrite(motorArray[i].dir_port, motorArray[i].dir_pin, motorArray[i].dir_pin);
		
		GPIOPinTypeGPIOOutput(motorArray[i].sleep_port, motorArray[i].sleep_pin);
		GPIOPinWrite(motorArray[i].sleep_port, motorArray[i].sleep_pin, ~motorArray[i].sleep_pin);
		
		GPIOPinTypeGPIOOutput(motorArray[i].step_port, motorArray[i].step_pin);
		GPIOPinWrite(motorArray[i].step_port, motorArray[i].step_pin, ~motorArray[i].step_pin);
		
	}
	
	
}




void FaultISR(void)
{
	uint32_t port = GPIO_PORTF_BASE;
	uint32_t pin  = GPIO_PIN_4;
	
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(port, pin);
	GPIOPinWrite(port, pin, pin);
    
}

//EOF
