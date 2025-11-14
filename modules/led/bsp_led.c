#include "bsp_led.h"

/*
 * LED1:PB11 LED2:PB12 PED3:PB13
*/
void bsp_led_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pins : PB11 PB12 PB13 */
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void bsp_led1_on()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);

}

void bsp_led1_off()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
}
void bsp_led1_toggle()
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
}

void bsp_led2_on()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}
void bsp_led2_off()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}
void bsp_led2_toggle()
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
}

void bsp_led3_on()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}

void bsp_led3_off()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
}
void bsp_led3_toggle()
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
}


