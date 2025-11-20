#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "bsp_led.h"

//栈溢出检测钩子函数
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* 用户自定义处理，比如打印、断言或重启 */
  
    /* 简单处理：进入死循环 */
   (void)xTask;
    (void)pcTaskName;
    while(1);
}

#if 1
static void StartDefaultTask(void *argument)
{
	while(1)
	{
		bsp_led1_toggle();
		vTaskDelay(500);
	}
}
#endif

void MX_FREERTOS_Init(void) {
	xTaskCreate(
		  StartDefaultTask,
		  "StartDefaultTask",
		  200,
		  NULL,
		  configMAX_PRIORITIES - 1,
		  NULL);

}




