#include "stm32f10x.h"                  // Device header

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15);
}

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void LED1_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	}
}
void LED3_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void LED3_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}
void LED4_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void LED4_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}
void LED2_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

void LED2_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_2) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	}
}
void light_dist(float dist){
if(dist<10){
LED1_ON();
LED2_OFF();
LED3_OFF();
LED4_OFF();
}
else if(dist>10&&dist<150){
LED1_ON();
LED2_ON();
LED3_OFF();
LED4_OFF();

}
	else if(dist>150&&dist<300){
LED1_ON();
LED2_ON();
LED3_ON();
LED4_OFF();
	}
	else if(dist>300){
LED1_ON();
LED2_ON();
LED3_ON();
LED4_ON();
	}
}