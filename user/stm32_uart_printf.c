#include "bsp_uart.h"
#include "device_manager.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define UART_RX_BUF_LEN 260
 
 extern UART_HandleTypeDef huart1;
 struct Dev_Mgmt g_uart1_dev;
 
 struct UART_Data {
	 UART_HandleTypeDef *huart;
	 GPIO_TypeDef* GPIOx_485;
	 uint16_t GPIO_Pin_485;
	 QueueHandle_t xRxQueue;
	 SemaphoreHandle_t xTxSem;
	 uint8_t rx_buf[UART_RX_BUF_LEN];		 
 };
 
 static struct UART_Data g_uart1_data = {
	.huart = &huart1,
	.GPIOx_485 = GPIOA,
	.GPIO_Pin_485 = GPIO_PIN_8,
	.xRxQueue = NULL,
	.xTxSem = NULL,
	.rx_buf = {0},
 };
 
 
 void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 {
	 if (huart == &huart1)
	 {
		 struct UART_Data * uart_data = g_uart1_dev.priv_data;		  
		 xSemaphoreGiveFromISR(uart_data->xTxSem, NULL);
		 HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_RESET);
	 }
 }
 
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 {
	 if (huart == &huart1)
	 {
		 struct UART_Data * uart_data = g_uart1_dev.priv_data; 
		 
		 for (int i = 0; i < UART_RX_BUF_LEN; i++)
		 {
			 xQueueSendFromISR(uart_data->xRxQueue, (const void *)&uart_data->rx_buf[i], NULL);
		 }
		 //HAL_UART_Receive_IT(uart_data->huart, &uart_data->rxdata, 1);
		 HAL_UARTEx_ReceiveToIdle_DMA(uart_data->huart, uart_data->rx_buf, UART_RX_BUF_LEN);
	 }		 
 }
 
 void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
 {
	 
	 if (huart == &huart1)
	 {
		 struct UART_Data * uart_data = g_uart1_dev.priv_data;		  
 
		 HAL_UART_DeInit(uart_data->huart);
		 HAL_UART_Init(uart_data->huart);
		 
		 //HAL_UART_Receive_IT(uart_data->huart, &uart_data->rxdata, 1);
		 HAL_UARTEx_ReceiveToIdle_DMA(uart_data->huart, uart_data->rx_buf, UART_RX_BUF_LEN);
	 }	  
 }
 
 
 /* Size: 本次启动的DMA传输接收到是数据的总个数
  */
 void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
 {
	 struct UART_Data * uart_data;
	 static  uint16_t old_pos = 0;
	 
	 if (huart == &huart1)
	 {
		 uart_data = g_uart1_dev.priv_data;  
	 }
	 else
	 {
		 return;
	 }
	 
	 /* write queue : g_uart4_rx_buf Size bytes ==> queue */
	 for (int i = old_pos; i < Size; i++)
	 {
		 xQueueSendFromISR(uart_data->xRxQueue, (const void *)&uart_data->rx_buf[i], NULL);
	 }
 
	 old_pos = Size;
 
	 if (HAL_UART_RXEVENT_HT != huart->RxEventType)
	 {
		 old_pos = 0;
		 
		 /* re-start DMA+IDLE rx */
		 HAL_UARTEx_ReceiveToIdle_DMA(uart_data->huart, uart_data->rx_buf, UART_RX_BUF_LEN);
	 }
		 
 }
 
 
 static int stm32_uart_init(struct Dev_Mgmt *pDev, int baud, char parity, int data_bit, int stop_bit)
 {
	 struct UART_Data * uart_data = pDev->priv_data;
	 
	 if (!uart_data->xRxQueue)
	 {
		 uart_data->xRxQueue = xQueueCreate(UART_RX_BUF_LEN, 1);
		 uart_data->xTxSem	 = xSemaphoreCreateBinary( );
 
		 /* 配置RS485转换芯片的方向引脚,让它输出0表示接收 */
		 HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_RESET);
		 
		 //HAL_UART_Receive_IT(uart_data->huart, &uart_data->rxdata, 1);
		 HAL_UARTEx_ReceiveToIdle_DMA(uart_data->huart, uart_data->rx_buf, UART_RX_BUF_LEN);
	 }
	 return 0;
 }
 
 static int stm32_uart_send(struct Dev_Mgmt *pDev, uint8_t *datas, uint32_t len, int timeout)
 {
	 struct UART_Data * uart_data = pDev->priv_data;
	 
	 /* 配置RS485转换芯片的方向引脚,让它输出1表示发送 */
	 HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_SET);
	 
	 //HAL_UART_Transmit_IT(uart_data->huart, datas, len);
	 HAL_UART_Transmit_DMA(uart_data->huart, datas, len);
	 
	 /* 等待1个信号量(为何不用mutex? 因为在中断里Give mutex会出错) */
	 if (pdTRUE == xSemaphoreTake(uart_data->xTxSem, timeout))
	 {
		 HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_RESET);
		 return 0;
	 }
	 else
	 {
		 HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_RESET);
		 return -1;
	 }
 }
 
 static int stm32_uart_recv(struct Dev_Mgmt *pDev, uint8_t *pData, int timeout)
 {
	 struct UART_Data * uart_data = pDev->priv_data;
	 if (pdPASS == xQueueReceive(uart_data->xRxQueue, pData, timeout))
		 return 0;
	 else
		 return -1;
 }
 
 
 static int stm32_uart_flush(struct Dev_Mgmt *pDev)
 {
	 struct UART_Data * uart_data = pDev->priv_data;
	 int cnt = 0;
	 uint8_t data;
	 while (1)
	 {
		 if (pdPASS != xQueueReceive(uart_data->xRxQueue, &data, 0))
			 break;
		 cnt++;
	 }
	 return cnt;
 }
 
 
 struct Dev_Mgmt g_uart1_dev = {"uart1", stm32_uart_init, stm32_uart_send, stm32_uart_recv, stm32_uart_flush, &g_uart1_data};



#if 1
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





