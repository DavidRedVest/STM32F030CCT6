#include "device_manager.h"
//#include <stdio.h>
#include <string.h>

extern struct Dev_Mgmt g_uart1_dev; 

static struct Dev_Mgmt *g_uart_devices[] = {&g_uart1_dev};

struct Dev_Mgmt *GetUARTDevice(char *name)
{
	unsigned int i = 0;
	
	while(g_uart_devices[i] != NULL) {
		if(g_uart_devices[i]->name && !strcmp(name, g_uart_devices[i]->name)) {
			return g_uart_devices[i];
		}
		i++;
	}
	
	return NULL;
}



