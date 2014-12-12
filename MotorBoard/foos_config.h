/*
* foos_config.h
*/

#ifndef FOOS_CONFIG_H
#define FOOS_CONFIG_H



#include <stdint.h>



#define TOTAL_MOTORS    2
#define ACC_ENABLE      0
#define PRINT_CALIBRATE 0



static volatile uint32_t criticalRegionCount = 0;



#define FOOS_ASSERT(a)                          \
{  												\
	if( !(a) )									\
	{											\
		char* text =  "Fail in: ";				\
		_write(0, text, strlen(text));			\
		_write(0, __FILE__, strlen(__FILE__));	\
		_write(0, "\n\r", strlen("\n\r"));      \
												\
		while(1)								\
			continue;							\
	}											\
}



#define CRITICAL_START()    \
{ 							\
	IntMasterDisable();     \
	criticalRegionCount++;  \
}



#define CRITICAL_END()               \
{						    		 \
	if(--criticalRegionCount == 0)   \
		IntMasterEnable();			 \
}



#endif


//EOF
