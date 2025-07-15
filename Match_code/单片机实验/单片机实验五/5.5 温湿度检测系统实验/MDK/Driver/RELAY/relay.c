#include "relay.h"

void RELAY_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_RELAY, ENABLE);	//ʹ�ܶ˿�ʱ��
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin   = RELAY_GPIO_PIN;
	GPIO_Init(RELAY_GPIO_PORT, &GPIO_InitStructure); //��ʼ��GPIO		
	GPIO_ResetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN);
}								    




