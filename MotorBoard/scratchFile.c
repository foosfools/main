#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "driverlib/rom_map.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
//#include "driverlib/pwm.h"
#include <string.h>
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"


static const uint32_t port = GPIO_PORTB_BASE;
static const uint32_t pin  = GPIO_PIN_2;



static void initGPIO(void)
{
	MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
    				120000000);
	//SysCtlDelay(5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(port, pin);
}


int main(void)
{
	bool toggleVal = true;
	
	enum
	{
		delay_ms = 1000,
	};
	
	initGPIO();
	GPIOPinWrite(port, pin, pin);
	
	for(;;)
	{

	//	SysCtlDelay(delay_ms);
	//	toggleVal = !toggleVal;
	}
	
	return 0;
}