#include <iostream>
#include <fcntl.h>   /* File control definitions */
#include "virtualPort.h"

using namespace std;

int main()
{
	static const uint8_t bufCount = 100;
	char * buf = "on\n\r";
	
	int fd = virtualPort_open();

	if(fd == -1)
	{
		return -1;
	}
	configure_port(fd);
	
	if(write(fd, buf, strlen(buf) + 1) != strlen(buf) + 1)
	{
	cout << "error" << endl;
	}
	
	cout << "written" << endl;
	
	while(1)
	{
	}
	return 0;
}