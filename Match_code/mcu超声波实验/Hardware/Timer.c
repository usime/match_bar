#include "Timer.h"
#include "stm32f10x.h"                
#include "stdint.h"
int CNT=0;
void Timer_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseInitSture;
	NVIC_InitTypeDef           NVIC_InitSture;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitSture.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitSture.TIM_Period=arr-1;
	TIM_TimeBaseInitSture.TIM_Prescaler=psc-1;
	TIM_TimeBaseInitSture.TIM_ClockDivision=0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitSture);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_Trigger,ENABLE);
	
	NVIC_InitSture.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitSture.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitSture.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitSture.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitSture);
	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
		
	}
}





void Timer2_init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);												//时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr;																//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;															//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;												//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;											//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);														//根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);															//使能中断允许位  DIER .BIT0

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;														//TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;											//抢占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;													//响应优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;														//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);																		//初始化NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);																				//使能TIM3			 
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		CNT++;
		
	}
}







