#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "OLED.h"

void Motor_Pin_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;								
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}	

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;								//想用A12,A4     B0,B1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_4;//应该四个引脚，左右电机的方向控制,假设12为左
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;								
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	PWM_Init();
}

//占空比Duty应该在0-7200之间比较合适

void SetDuty(int16_t Left_Duty,int16_t Right_Duty)
{
	if(Left_Duty==0)
	{
	GPIO_SetBits(GPIOA,GPIO_Pin_12);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	}
	if(Right_Duty==0)
	{
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	}
	if(Left_Duty>0)
	{
	GPIO_SetBits(GPIOA,GPIO_Pin_12);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	PWM_SetCompare1(Left_Duty);
	}
	if(Left_Duty<0)
	{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	PWM_SetCompare1(-Left_Duty);
	}	
	if(Right_Duty>0)
	{
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	PWM_SetCompare2(Right_Duty);
	}
	if(Right_Duty<0)
	{
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	PWM_SetCompare2(-Right_Duty);
	}
}

