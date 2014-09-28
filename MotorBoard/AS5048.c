/*
* AS5048.c
*/

#include "AS5048.h"
#include "hal2.h"
#include "foos_spi.h" 

void AS5048_init(motor_foop* motorArray, uint8_t totalMotors)
{
	spi_init(motorArray, totalMotors);
}

uint16_t gitParityBit(uint16_t data)
{
	int parity = 0; 
	for(int i = 0; i < 14; i++)
	{
		if(data & (1<<i))
			parity++; 
	}
	
	if(parity%2 == 0)
		return 1<<15; 
	return 0; 
}



static uint16_t read_AS5048 (uint32_t port, uint32_t pin, address_t address)
{
	uint16_t buf = address | read_command;
	
	buf |= gitParityBit(buf); 
	spi_open(port, pin); 
	spi_write16(&buf, 1); 
	spi_close(port, pin); 
	
	for(volatile int i = 0; i<20; i++) //busy loop
	{
		continue; 
	}
	
	uint32_t result = 0; 
	spi_open(port, pin); 
	spi_write16(&buf, 1); 
	//	spi_read((uint8_t*)&result, 2); 
	while(SSIDataGetNonBlocking(SSI0_BASE, &result))
		continue;
	spi_close(port, pin); 
	
	

	
	if(error_bit & result)
	{
		errorFlag_clearrr(port, pin);
	}
	
	return (uint16_t)result & 0x3FFF;
}



uint16_t AS5048_readMagnitude(uint32_t port, uint32_t pin)
{
	return read_AS5048(port, pin, magnitudeReg_address);// & 0x3FFF;
}



uint16_t AS5048_readAngle(uint32_t port, uint32_t pin)
{
	return read_AS5048(port, pin, angleReg_address);// & 0x3FFF;
}

void errorFlag_clearrr(uint32_t port, uint32_t pin)
{
	uint16_t buf = 0x0001 | read_command;
	
	buf |= gitParityBit(buf); 
	spi_open(port, pin); 
	spi_write16(&buf, 1); 
	spi_close(port, pin); 
	
	for(volatile int i = 0; i<20; i++) //busy loop
	{
		continue; 
	}
	
	spi_open(port, pin); 
	//spi_read((uint8_t*)&result, 2); 
	spi_write16(&buf, 1); 
	spi_close(port, pin); 
	
}

//EOF
