/*
* foos_spi.h
*/
#ifndef FOOS_SPI_H
#define FOOS_SPI_H

#include <stdint.h>
#include "driverlib/ssi.h"

typedef enum
{
	spi0,
	spi1,
} spi_num;

//enables MISO, MOSI, and CLK for specific spi port
void spiPort_init(spi_num spi);

//enables SS for spi
void spiGPIO_init(uint32_t port, uint32_t pin);

//open must be used prior to read angle or mag
void spi_open(uint32_t port, uint32_t pin);

//spi must be closed
void spi_close(uint32_t port, uint32_t pin);

//buf - the data to be written
//size- the size in bytes of the data to be written
void spi_write16(spi_num spi, uint16_t* buf, size_t size);

//buf - the buffer where data gets written
//size- the size in bytes of the data to be read
//returns - size of data actually read
//NOTE: DEPRECATED
size_t spi_read(uint8_t* buf, size_t size);

#endif

//EOF
