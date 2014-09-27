/*
* AS5048.c
*/

#include "AS5048.h"
#include "hal.h"
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
	uint16_t buf = spi_write(address | read_command);
	buf |= gitParityBit(buf); 
	spi_open(port, pin); 
	spi_write(buf, 2); 
	spi_close(port, pin); 
	for(volatile int i = 0; i<20; i++) //busy loop
	{
		continue; 
	}
	unint16_t result = 0; 
	spi_open(port, pin); 
	spi_read(&result, 2); 
	spi_close(port, pin); 
	return result
}

uint16_t AS5048_readMagnitude(uint32_t port, uint32_t pin)
{
	return read_AS5048(port, pin, magnitudeReg_address) & 0x3FFF;
}

uint16_t AS5048_readAngle(uint32_t port, uint32_t pin)
{
	return read_AS5048(port, pin, angleReg_address) & 0x3FFF;
}
//EOF