#include "timer.h"

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器2!
void Tim3_init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);												//时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr;																//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;															//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;												//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;											//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);														//根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);															//使能中断允许位  DIER .BIT0

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;														//TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;											//抢占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;													//响应优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;														//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);																		//初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);																				//使能TIM3			 
}

////定时器3中断服务程序
//void TIM3_IRQHandler(void)   
//{
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update))																//更新中断
//	{
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//	}
//}












