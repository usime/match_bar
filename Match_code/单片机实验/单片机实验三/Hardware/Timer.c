#include "Timer.h"
#include "stm32f10x.h"                
#include "stdint.h"
#include "multibutton.h"
uint16_t CNT=0;
#include "stm32f10x.h"                  // Device header
#include "SR03.h"
#include "PID.h"
float Angle;
extern uint16_t Pulse_CNT;
uint16_t Current_pulse=0;
uint16_t Last_Pulse_CNT=0;
uint16_t pulse_diff=0;
int Speed=0;
uint32_t fliter_value=0;
int median_fliter_arr[50]={0};
extern uint8_t Turn_mode_flag;
int median_index=0;
uint32_t Ms_50_cnt=0;
uint32_t Actual_speed=0;
void TIM3_PWM_Init()
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;

      
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
     
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
                                                                          
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
                                                                          
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;			
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_Period = 2000- 1;				
	TIM_TimeBaseInitStructure.TIM_Prescaler = 100- 1;			
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;         
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);           
	TIM_OCStructInit(&TIM_OCInitStructure);						
												
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = 0;		
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_Cmd(TIM3, ENABLE);			
}



void PWM_SetCompar4(int16_t Compare)
{
	TIM_SetCompare4(TIM3, Compare);		//设置CCR4的值
}

void PWM_SetCompare3(int16_t Compare)
{
	TIM_SetCompare3(TIM3, Compare);		//设置CCR3的值
}
void PWM_SetCompare2(int16_t Compare)
{
	TIM_SetCompare2(TIM3, Compare);		//设置CCR2的值
}






void Timer2_init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);												//时钟使能
	

	TIM_TimeBaseStructure.TIM_Period = arr-1;																//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1;															//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;												//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;											//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);														//根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);															//使能中断允许位  DIER .BIT0


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
		if(Turn_mode_flag==0){
		CNT++;	
		Current_pulse=Pulse_CNT;
		if(CNT>=50){
		Ms_50_cnt++;
		Speed =(60*Current_pulse)/Ms_50_cnt*0.17;
		Actual_speed=Speed_pid_realize(&Motor_Ctrl,Speed);
		CNT=0;
		}
		}
		else{
		CNT=0;
		Ms_50_cnt=0;
		Pulse_CNT=0;	
		}

		button_ticks();
	}
}







