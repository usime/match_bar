#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
    // 1. 使能时钟
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 配置GPIO（PA8-CH1、PA9-CH2）
	GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

	
	
	TIM_InternalClockConfig(TIM1);
    // 3. 配置TIM1时基
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_TimeBaseStruct.TIM_Period = 7200-1;          // ARR=7200
    TIM_TimeBaseStruct.TIM_Prescaler = 10-1;        // PSC=10 此时频率为1kHz
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    // 4. 配置PWM输出模式（通道1和通道2）
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);                         //结构体初始化，若结构体没有完整赋值
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;             // 初始占空比0
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);        // 通道1初始化
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);        // 通道2初始化

    // 5. 高级定时器需使能主输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // 6. 启动定时器
    TIM_Cmd(TIM1, ENABLE);
}

//修改占空比函数，值的范围为0-7200
void PWM_SetCompare1(uint16_t compare)
{
	TIM_SetCompare1(TIM1,compare);  
}
 void PWM_SetCompare2(uint16_t compare)
{
	 TIM_SetCompare2(TIM1, compare);  
}
       


