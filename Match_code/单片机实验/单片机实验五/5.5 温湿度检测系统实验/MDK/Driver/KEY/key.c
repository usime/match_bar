#include "key.h"

void KEY_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_KEY1, ENABLE);		//使能端口时钟
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = KEY1_GPIO_PIN;
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure); 	

	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_KEY2, ENABLE);		//使能端口时钟
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = KEY2_GPIO_PIN;
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure); 

	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_KEY3, ENABLE);		//使能端口时钟
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = KEY3_GPIO_PIN;
	GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure); 	
}								    


