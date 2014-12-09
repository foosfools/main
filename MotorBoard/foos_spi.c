/*
* foos_spi.c
*/

#include <stdint.h>
#include "hal2.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "AS5048.h"
#include "foos_spi.h"


void spiPort_init(spi_num spi)
{
	enum
	{
		spiFreq = 300000,
	};
	
	uint32_t spi_base = 0;
	
	switch( spi )
	{
		case spi0:
			spi_base = SSI0_BASE;
			SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
			GPIOPinConfigure(GPIO_PA2_SSI0CLK);
			GPIOPinConfigure(GPIO_PA4_SSI0XDAT0); //MOSI
			GPIOPinConfigure(GPIO_PA5_SSI0XDAT1); //MISO

			SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1,
							   SSI_MODE_MASTER, spiFreq, 16);
			SSIEnable(SSI0_BASE);
   
			GPIOPinTypeSSI(GPIO_PORTA_BASE,  GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);
			break;
			
		case spi1:
		    spi_base = SSI1_BASE;
			SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	
			GPIOPinConfigure(GPIO_PB5_SSI1CLK);
			GPIOPinConfigure(GPIO_PE4_SSI1XDAT0); //MOSI
			GPIOPinConfigure(GPIO_PE5_SSI1XDAT1); //MISO

			SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1,
							   SSI_MODE_MASTER, spiFreq, 16);
			SSIEnable(SSI1_BASE);
   
			GPIOPinTypeSSI(GPIO_PORTE_BASE,  GPIO_PIN_4 | GPIO_PIN_5);
			GPIOPinTypeSSI(GPIO_PORTB_BASE,  GPIO_PIN_5);
			break;
   }
   
   uint32_t dummyBuf;
	while(SSIDataGetNonBlocking(spi_base, &dummyBuf)) // clears FIFOs
		continue;
}



void spiGPIO_init(uint32_t port, uint32_t pin)
{
	 GPIOPinTypeGPIOOutput(port, pin);
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
void spi_write16(spi_num spi, uint16_t* buf, size_t size)
{
	uint32_t temp = 0;
	uint32_t spi_base = 0;
	
	switch( spi )
	{
		case spi0:
			spi_base = SSI0_BASE;
			break;
			
		case spi1:
			spi_base = SSI1_BASE;
			break;
	}
	
	for(uint32_t i = 0; i < size; i++)
	{
		temp = (uint32_t)buf[i];
        SSIDataPut(spi_base, temp);
	}
	
	
	while(SSIBusy(spi_base))
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