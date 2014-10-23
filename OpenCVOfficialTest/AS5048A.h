#include <stdint.h>
#include "hal.h"

//slave selects need to be enabled separately
void spi_init(vmotor_foop* motorArray, uint32_t totalMotors);

//open must be used prior to read angle or mag
void spi_open(uint32_t port, uint32_t pin);


//spi must be closed
void spi_close(uint32_t port, uint32_t pin);


//buf - the data to be written
//size- the size in bytes of the data to be written
void spi_write(uint8_t* buf, size_t size);


//buf - the buffer where data gets written
//size- the size in bytes of the data to be read
//returns - size of data actually read
size_t spi_read(uint8_t* buf, size_t size);



//EOF
