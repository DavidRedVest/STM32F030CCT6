#include "bsp_uart.h"

#if 0
 /* 为了实现printf打印，需要实现一个myputstr函数 */
static int myputchar(const char ch)
{
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR = (uint8_t) ch;      

	return ch;	
}

void myputstr(const char *str)
{
	while(*str) {
		myputchar(*str);
		str++;
	}

}
#endif


