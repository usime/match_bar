#ifndef __MOTOR_H
#define __MOTOR_H
#include <stm32f10x.h>
#include "SYS.h"


/* 定义 MOTOR 引脚 */
#define 	RCC_AHBxPeriph_MOTOR_EN1	RCC_APB2Periph_GPIOB
#define 	MOTOR_EN1_GPIO_PORT			GPIOB
#define 	MOTOR_EN1_GPIO_PIN			GPIO_Pin_8
#define 	MOTOR_EN1					PBout(8)

#define 	RCC_AHBxPeriph_MOTOR_IN1	RCC_APB2Periph_GPIOB
#define 	MOTOR_IN1_GPIO_PORT			GPIOB
#define 	MOTOR_IN1_GPIO_PIN			GPIO_Pin_1
#define 	MOTOR_IN1					PBout(1)

#define 	RCC_AHBxPeriph_MOTOR_IN2	RCC_APB2Periph_GPIOB
#define 	MOTOR_IN2_GPIO_PORT			GPIOB
#define 	MOTOR_IN2_GPIO_PIN			GPIO_Pin_9
#define 	MOTOR_IN2					PBout(9)


//PWM输出口配置  这里配置TIM3的通道3。
#define 	RCC_APB1Periph_PWM			RCC_APB1Periph_TIM3
#define 	PWM_TIM						TIM3
#define  	PWM_TIM_OCXInit 			TIM_OC4Init
#define  	PWM_TIM_OCXPreloadConfig	TIM_OC4PreloadConfig
#define  	PWM_TIM_SetCompareX			TIM_SetCompare4

#define 	RCC_APB2Periph_PWM			RCC_APB2Periph_GPIOB
#define 	PWM_GPIO_PORT				GPIOB
#define  	PWM_GPIO_PIN				GPIO_Pin_1


//PLU捕获口配置  这里配置TIM2的通道2。
#define 	RCC_APB1Periph_PLU			RCC_APB1Periph_TIM2
#define 	PLU_TIM						TIM2
#define  	PLU_TIM_Channel_X 			TIM_Channel_2
#define  	PLU_TIM_IT_CCX 				TIM_IT_CC2
#define  	PLU_TIMX_IRQn				TIM2_IRQn
#define  	PLU_TIMX_IRQHandler			TIM2_IRQHandler

#define 	RCC_APB2Periph_PLU			RCC_APB2Periph_GPIOA
#define 	PLU_GPIO_PORT				GPIOA
#define  	PLU_GPIO_PIN				GPIO_Pin_1


void MOTOR_init(void);											//电机GPIO引脚初始化
void MOTOR_control(unsigned int control);			//电机控制

void MOTOR_PWM_init(unsigned int frequency,unsigned int zkb);	//传递频率参数	占空比参数
void MOTOR_PWM_control(unsigned int control,unsigned int zkb);			//电机PWM控制

void MOTOR_PLU_init(unsigned int ms);		//电机脉冲捕获初始化
	

extern unsigned long PLU_count;//全局定时采集的脉冲信号
extern unsigned char PLU_flag; //全局定时采集的脉冲信号完成标志位

#endif
