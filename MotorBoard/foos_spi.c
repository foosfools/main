/*
* foos_spi.c
*/

#include <stdint.h>
#include "hal2.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "AS5048.h"



void spi_init(motor_foop* motorArray, uint32_t totalMotors)
{
	volatile uint32_t clock = SysCtlClockGet();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
	/*
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    */
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1,
                       SSI_MODE_MASTER, 300000, 16);
   SSIEnable(SSI0_BASE);
   
  GPIOPinTypeSSI(GPIO_PORTA_BASE,  GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);
   
   //tx, rx, clk
   for(uint32_t i = 0; i < totalMotors; i++)
   {
	 // GPIOPinTypeSSI(motorArray[i].slaveSel_port, motorArray[i].slaveSel_pin);
	 GPIOPinTypeGPIOOutput(motorArray[i].slaveSel_port, motorArray[i].slaveSel_pin);
   }
   

   uint32_t dummyBuf;
	while(SSIDataGetNonBlocking(SSI0_BASE, &dummyBuf)) // clears FIFOs
		continue;
}



//open must be used prior to read angle or mag
void spi_open(uint32_t port, uint32_t pin)
{
	GPIOPinWrite(port, pin, ~pin);
}



//spi must be closed
void spi_close(uint32_t port, uint32_t pin)
{
	GPIOPinWrite(port, pin, pin);
}



//buf - the data to be written
//size- the number of 16 bit values to send
//assumes spi has been opened
void spi_write16(uint16_t* buf, size_t size)
{
	uint32_t temp = 0;
	
	for(uint32_t i = 0; i < size; i++)
	{
		temp = (uint32_t)buf[i];
        SSIDataPut(SSI0_BASE, temp);
	}
	
	
	while(SSIBusy(SSI0_BASE))
		continue;
}



//buf - the buffer where data gets written
//size- the size in bytes of the data to be read
//returns - size of data actually read
size_t spi_read(uint8_t* buf, size_t size)
{
	uint32_t tempBuf = 0;
	
	for(uint32_t i = 0; i < size; i++)
	{
		SSIDataGet (SSI0_BASE, &tempBuf);
		buf[i] = (uint8_t)(tempBuf & 0xFF);
	}
	
	while(SSIBusy(SSI0_BASE))
		continue;
		
	return size;
}



//EOF