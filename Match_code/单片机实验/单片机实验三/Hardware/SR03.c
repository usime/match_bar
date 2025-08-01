#include "SR03.h"
#include "delay.h"
#include "stm32f10x.h"             
#define Echo GPIO_Pin_2
#define Trig GPIO_Pin_3
#define FILTER_WINDOW_SIZE 10
int Distance;
extern float speed;
void SR03_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitSture;
	EXTI_InitTypeDef  EXTI_InitSture;
	NVIC_InitTypeDef  NVIC_InitSture;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_Out_PP;   
	GPIO_InitSture.GPIO_Pin=Trig;               
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA,&GPIO_InitSture);
	GPIO_InitSture.GPIO_Mode=GPIO_Mode_IPD;  
	GPIO_InitSture.GPIO_Pin=Echo;                
	GPIO_InitSture.GPIO_Speed=GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA,&GPIO_InitSture);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);
	EXTI_InitSture.EXTI_Line=EXTI_Line2;
	EXTI_InitSture.EXTI_LineCmd=ENABLE;
	EXTI_InitSture.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitSture.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitSture);
	NVIC_InitSture.NVIC_IRQChannel=EXTI2_IRQn;
	NVIC_InitSture.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitSture.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitSture.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitSture);
}
int median_filter(int Data, int median_buffer[], int* buffer_index) {
    static int init = 0;
    if (!init) {
        for (int i = 0; i < FILTER_WINDOW_SIZE; ++i) {
            median_buffer[i] = Data;
        }
        init = 1;
    }
    median_buffer[(*buffer_index)++] = Data;
    if (*buffer_index >= FILTER_WINDOW_SIZE) {
        *buffer_index = 0;
    }
    // 对缓冲区进行排序
    int temp_buffer[FILTER_WINDOW_SIZE];
    for (int i = 0; i < FILTER_WINDOW_SIZE; ++i) {
        temp_buffer[i] = median_buffer[i];
    }
    for (int i = 0; i < FILTER_WINDOW_SIZE - 1; ++i) {
        for (int j = 0; j < FILTER_WINDOW_SIZE - 1 - i; ++j) {
            if (temp_buffer[j] > temp_buffer[j + 1]) {
                int temp = temp_buffer[j];
                temp_buffer[j] = temp_buffer[j + 1];
                temp_buffer[j + 1] = temp;
            }
        }
    }

    // 取中间值作为中值滤波结果
    int median = temp_buffer[FILTER_WINDOW_SIZE / 2];
    return median;
}
int moving_filter(int Data, int filter_buffer[], int* buffer_index) {
    static int init = 0;  
    if (!init) {
        for (int i = 0; i < FILTER_WINDOW_SIZE; ++i) {
            filter_buffer[i] = Data;
        }
        init = 1;
    }
    filter_buffer[(*buffer_index)++] = Data;
    if (*buffer_index >= FILTER_WINDOW_SIZE) {
        *buffer_index = 0; 
    }
    float sum = 0;
    for (int i = 0; i < FILTER_WINDOW_SIZE; ++i) {
        sum += filter_buffer[i];
    }
    return sum / FILTER_WINDOW_SIZE;  // 返回平均值
}
//void EXTI2_IRQHandler(void)
//{
//Delay_us(10);
//if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
//{
//	TIM_SetCounter(TIM3,0);
//	TIM_Cmd(TIM3,ENABLE);
//	while(GPIO_ReadInputDataBit(GPIOA,Echo));  
//	TIM_Cmd(TIM3,DISABLE);
//	Distance=TIM_GetCounter(TIM3)*speed/2.0;	//这里需要改一下，需要温度传感器结合波速的关系。

//	EXTI_ClearITPendingBit(EXTI_Line2);
//}
//}

void SR03_Strat(void)
{
	GPIO_SetBits(GPIOA,Trig);  
	Delay_us(10);              
	GPIO_ResetBits(GPIOA,Trig); 
	
}
float linear_compensation_function(float measured_distance) {
    // 根据提供的数据点拟合得到的线性补偿函数
    // 这里使用的是一个简单的线性回归模型
    float slope = 1.4;  // 斜率
    float intercept = -0.5;  // 截距

    // 补偿后的距离 = slope * measured_distance + intercept
    return slope * measured_distance + intercept;
}





