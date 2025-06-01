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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO,ENABLE);//¿ªƴʱ֓
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);//³õʼ»¯¶¨ʱƷ¡£
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=10000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=100-1;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);/*¡¾2¡¿*///TIM2
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);							//�ṹ���ʼ�������ṹ��û��������ֵ
											
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//����Ƚ�ģʽ��ѡ��PWMģʽ1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//������ԣ�ѡ��Ϊ�ߣ���ѡ����Ϊ�ͣ�������ߵ͵�ƽȡ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//���ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;								//��ʼ��CCRֵ
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//¸߼¶¶¨ʱƷרʴ--MOEַʤ³öʹĜ
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);/*¡¾3¡¿*///ENABLE//OC1ԤװԘ¼Ĵ憷ʹ�?
  TIM_Cmd(TIM1,ENABLE);//¿ª¶¨ʱƷ¡£
}
void Set_Beep_compare(int Compare){
TIM_SetCompare1(TIM1,Compare);
}

int Dist_tocompare(float dist){
return 10.6*dist+2000;
}