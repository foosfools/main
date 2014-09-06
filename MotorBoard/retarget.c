/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include <string.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
//#pragma import(__use_no_semihosting_swi)


//extern int  sendchar(int ch);  /* in serial.c */


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;





int fputc(int ch, FILE *f) {
	UARTCharPutNonBlocking(UART0_BASE, (char) ch);
  return 1;
}

int _write(int file, char* ptr, int len)
{
	int i = 0;
	
	for(;i < len; i++)
	{
		if(*ptr == 0)
		{
			break;
		}
		UARTCharPutNonBlocking(UART0_BASE, (char) ptr[0]);
		ptr++;
	}
	return len; 
}

/*
int ferror(FILE *f) {
  return EOF;
}
*/
/*
void _ttywrch(int ch) {
  sendchar(ch);
}
*/

void _exit(int return_code) {
label:  goto label;  /* endless loop */
}