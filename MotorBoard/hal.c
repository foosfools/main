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
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 1000);
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



void systemInit(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
					 SYSCTL_XTAL_16MHZ);
	UARTInit();
	stepSizePinsInit();
	PWMInit();
	TimerInit();
}



void PWMInit(void)
{
		uint32_t prescale = 8;
		SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(GPIO_PB6_M0PWM0);

    GPIOPinTypePWM(motor0_port, motor0_pin);
	  
		PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	  //N = (1 / f) * SysClk
	  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 64000 / prescale);
		//50% duty cycle
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 32000 / prescale);
	
		IntMasterEnable();

		 //
    // Enable the PWM0 output signal (PD0).
    //
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true); //motor0

    //
    // Enables the PWM generator block.
    //
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


//EOF
