/*
* hal.c
*/

#include "hal.h"
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
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 4000);
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

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
	  IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}



void systemInit(motor_foop* motorArray, uint8_t totalMotors)
{
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
					 SYSCTL_XTAL_16MHZ);
	UARTInit();
	stepSizePinsInit();
	PWMInit(motorArray, totalMotors);
	TimerInit();
	motorSleepDirPinsInit(motorArray, totalMotors);
}



void PWMInit(motor_foop* motorArray, uint32_t totalMotors)
{
	uint32_t prescale = 1;
		
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
	
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // motor 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // motor 1
	
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    GPIOPinConfigure(GPIO_PB6_M0PWM0); // motor 0 PWM
    GPIOPinConfigure(GPIO_PB7_M0PWM1); // motor 1
    
    for(int i = 0; i < totalMotors; i++)
    {
    	GPIOPinTypePWM(motorArray[i].step_port, motorArray[i].step_pin);
    }
    
	PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
                    
	  //N = (1 / f) * SysClk
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 64000 / prescale); // motor 0
	
		//50% duty cycle
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 32000 / prescale); // motor 0
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 32000 / prescale); // motor 1
	IntMasterEnable();

	for(int i = 0; i < totalMotors; i++)
    {
		PWMOutputState(motorArray[i].pwm_base, motorArray[i].pwm_outBit, false); //motor0
    }

    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
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



void MOTOR_ENABLE(uint32_t num, uint32_t time_ms, motor_foop * motorArray, bool direction)												    
{
	CRITICAL_START()	
	{						
															 									
		GPIOPinWrite(motorArray[num].dir_port, 
					motorArray[num].dir_pin, 
					(direction) ? motorArray[num].dir_pin : ~motorArray[num].dir_pin);
		GPIOPinWrite(motorArray[num].sleep_port, motorArray[num].sleep_pin, motorArray[num].sleep_pin);
		motorArray[num].remainTime_ms = time_ms;	
		PWMOutputState(motorArray[num].pwm_base, motorArray[num].pwm_outBit, true);	
		motorArray[num].pwm_en = true;  	
	}	
	CRITICAL_END()							
}



void MOTOR_DISABLE(uint32_t num,  motor_foop * motorArray)														    
{		
	CRITICAL_START()	
	{																		
		motorArray[num].pwm_en = false;   												
		PWMOutputState(motorArray[num].pwm_base, motorArray[num].pwm_outBit, false);	
		motorArray[num].remainTime_ms = 0;	
		GPIOPinWrite(motorArray[num].sleep_port, motorArray[num].sleep_pin, ~(motorArray[num].sleep_pin));	
	}	
	CRITICAL_END()			     			
}



void motorSleepDirPinsInit(motor_foop* motorArray, uint8_t totalMotors)
{
	for(int i = 0; i < totalMotors; i++)
	{
		GPIOPinTypeGPIOOutput(motorArray[i].dir_port, motorArray[i].dir_pin);
		GPIOPinWrite(motorArray[i].dir_port, motorArray[i].dir_pin, motorArray[i].dir_pin);
		
		GPIOPinTypeGPIOOutput(motorArray[i].sleep_port, motorArray[i].sleep_pin);
		GPIOPinWrite(motorArray[i].sleep_port, motorArray[i].sleep_pin, ~motorArray[i].sleep_pin);
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



//EOF
