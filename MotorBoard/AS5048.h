/*
* AS5048.h
*/

typedef enum
{
	read_command = 0x4000,
	write_command = 0x0000,
} command; 

typedef enum
{
	magnitudeReg_address = 0x3FFE,
	angleReg_address 	= 0x3FFF, 
} address_t; 

void AS5048_init(motor_foop* motorArray, uint8_t totalMotors);
uint16_t gitParityBit(uint16_t data);
static uint16_t read_AS5048 (uint32_t port, uint32_t pin, address_t address);
uint16_t AS5048_readMagnitude(uint32_t port, uint32_t pin);
uint16_t AS5048_readAngle(uint32_t port, uint32_t pin);

//EOF
