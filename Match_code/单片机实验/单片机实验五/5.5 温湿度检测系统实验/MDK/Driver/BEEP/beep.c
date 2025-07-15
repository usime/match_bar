#include "beep.h"

void BEEP_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_BEEP, ENABLE);	//使能端口时钟
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = BEEP_GPIO_PIN;
	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure); //初始化GPIO		
	GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}	

void BEEP_ON(){
GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}

void BEEP_OFF(){
GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
}




