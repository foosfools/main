#include "virtualPort.h" 
#include <iostream>
#include <fcntl.h> 

int main(void)
{
	int fd = virtualPort_open();
	virtualPort_configure(fd); 
	read(); 
}