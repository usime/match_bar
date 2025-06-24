#include "stm32f10x.h"                  // Device header
#define max_dist 780  //cm
#define min_dist 30		//cm
#define initial_val 2000
int compare=0;


void Beep_Init(){
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 配置为复用推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
  TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period=10000-1;
  TIM_TimeBaseInitStruct.TIM_Prescaler=100-1;
  TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = 0;
  TIM_OC1Init(TIM1, &TIM_OCInitStruct);
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
  TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_Cmd(TIM1,ENABLE);
}
void Set_Beep_compare(int Compare){
TIM_SetCompare1(TIM1,Compare);
}

int Dist_tocompare(int dist){
	compare=86.6*dist+2000;
  if(compare<0)compare=0;
	else if(compare>10000)compare=10000;
	return compare;
}