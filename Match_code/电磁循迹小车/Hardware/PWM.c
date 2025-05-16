#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
    // 1. ʹ��ʱ��
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

    // 2. ����GPIO��PA8-CH1��PA9-CH2��
	GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // �����������
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

	
	
	TIM_InternalClockConfig(TIM1);
    // 3. ����TIM1ʱ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_TimeBaseStruct.TIM_Period = 7200-1;          // ARR=7200
    TIM_TimeBaseStruct.TIM_Prescaler = 10-1;        // PSC=10 ��ʱƵ��Ϊ1kHz
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;            //�ظ����������߼���ʱ���Ż��õ�
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    // 4. ����PWM���ģʽ��ͨ��1��ͨ��2��
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);                         //�ṹ���ʼ�������ṹ��û��������ֵ
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;             // ��ʼռ�ձ�0
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);        // ͨ��1��ʼ��
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);        // ͨ��2��ʼ��

    // 5. �߼���ʱ����ʹ�������
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // 6. ������ʱ��
    TIM_Cmd(TIM1, ENABLE);
}

//�޸�ռ�ձȺ�����ֵ�ķ�ΧΪ0-7200
void PWM_SetCompare1(uint16_t compare)
{
	TIM_SetCompare1(TIM1,compare);  
}
 void PWM_SetCompare2(uint16_t compare)
{
	 TIM_SetCompare2(TIM1, compare);  
}
       


