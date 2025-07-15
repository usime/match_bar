#include "LED.h"

void LED_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_LED, ENABLE);			//使能端口时钟
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin   = LED_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure); 				//初始化GPIO		
	GPIO_ResetBits(LED_GPIO_PORT,LED_GPIO_PIN);
}								    




