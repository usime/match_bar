#include "stm32f10x.h"                  // Device header
#define max_dist 780  //cm
#define min_dist 30		//cm
#define initial_val 2000
void Beep_Init(){
  GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO,ENABLE);//驴陋拼时謸
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);//鲁玫始禄炉露篓时品隆拢
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=10000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=100-1;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);/*隆戮2隆驴*///TIM2
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);							//结构体初始化，若结构体没有完整赋值
											
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//赂呒露露篓时品专蚀--MOE址胜鲁枚使臏
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);/*隆戮3隆驴*///ENABLE//OC1预装詷录拇鎲肥鼓?
  TIM_Cmd(TIM1,ENABLE);//驴陋露篓时品隆拢
}
void Set_Beep_compare(int Compare){
TIM_SetCompare1(TIM1,Compare);
}

int Dist_tocompare(float dist){
return 10.6*dist+2000;
}