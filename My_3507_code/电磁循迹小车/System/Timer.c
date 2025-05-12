#include "stm32f10x.h"                  // Device header
#include "MPU6050.h"
#include "inv_mpu.h"
#include "Encoder.h"
#include "Pid.h"
#include "Motor.h"
#include "Sensor.h"

#define basic 4000
extern Sensor_Data Sensor;
int cnt=0;
int L_Speed;
int R_Speed;
float AverageSpeed;
int L_Duty=0;
int R_Duty=0;

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 2000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;//��ʱƵ��Ϊ50Hz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

//������Ϊ�̶����ڵ�pid����
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
//		//Ҫ����ADC�Ĵ��룬��ȡ��·������
		ADC_fliter();//�����˲�����
		circulate_location();//����λ��
		Angle_Pid(&Sensor);//pid����
//		L_Speed=Left_Encoder_Get();
//		R_Speed=Right_Encoder_Get();
		cnt++;//��ʱ����һ����־���֣������ж϶�ʱ���Ƿ���������
		SetDuty(basic+L_Duty,basic-R_Duty);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

int Get_L_Speed(void)
{
	return L_Speed;
}

int Get_R_Speed(void)
{
	return R_Speed;
}

int Get_L_Duty(void)
{
	return L_Duty;
}

int Get_R_Duty(void)
{
	return R_Duty;
}



