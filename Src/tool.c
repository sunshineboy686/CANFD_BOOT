
/********************************************************************************
* Include Headers
********************************************************************************/

#include    <stdio.h>

#include <string.h>
#include    <stdarg.h>

#include "tool.h"
#include "usb_data.h"






int MyPrintf(char * format, ...)
{
 	int n = 0;
#ifdef DEBUG_TRACE_EN
 	char  buf[256];
 	va_list ap;

 	va_start(ap,format);
	n = vsprintf((char *)buf, (const char*)format, ap);
	//Uart_send(2, (void *)buf, n);
	//Uart_send(UART_DEBUG_PRINT, (void *)buf, n);
 	Usb_Pro_Pack(0x08ff,(void *)buf,n);
	va_end(ap);
 #endif
 	return n;
}

void GetSysTick(uint32_t* n)
{
	*n =  HAL_GetTick();
}




uint8_t CheckSysTick(uint32_t* n,uint32_t time)
{
	uint8_t ret;
	uint32_t tick;
  uint32_t TempValue,MinusValue;

	
	ret = 0;
	tick = HAL_GetTick();
	TempValue=*n;

	
	MinusValue=(tick >= TempValue) ?(tick -TempValue)  : (0xFFFFFFFF - TempValue + tick );
	
	if(MinusValue >=  time)
	{
		ret = 1;

		*n += time;
	
		
	}
	return ret;
}
