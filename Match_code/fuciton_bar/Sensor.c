#include "stm32f10x.h"                  // Device header
#include "Sensor.h"

#define LEFT_MAX   380  // 左电感在导线正上方的ADC值
#define LEFT_MIN   20    // 左电感远离导线时的ADC值
#define MIDDLE_MAX 330
#define MIDDLE_MIN 170
#define RIGHT_MAX  400
#define RIGHT_MIN  20

Sensor_Data Sensor;

//通过此函数滤波
void ADC_fliter(void)
{
	u8 cow,column,i,j;	
	u16 sum=0;
	cow=10;//行数
	column=3;//列数

	for(i=0;i<column;i++)
	{
		for(j=0;j<cow;j++)
		{
			sum=sum+Sensor.ADCConvertedValue[j][i];
		}
	Sensor.ADC_filter_Value[i]=sum/10;	
	sum=0;
	}		
}
	
//通过此函数归一化处理得到小车的位置	
void circulate_location(void)
{
Sensor.Left = 100 * (Sensor.ADC_filter_Value[0] - LEFT_MIN) / (LEFT_MAX - LEFT_MIN);    // 左电感ADC值
Sensor.Middle = 100 * (Sensor.ADC_filter_Value[1] - MIDDLE_MIN) / (MIDDLE_MAX - MIDDLE_MIN);   // 中电感ADC值
Sensor.Right =100 * (Sensor.ADC_filter_Value[2] - RIGHT_MIN) / (RIGHT_MAX - RIGHT_MIN); // 右电感ADC值
Sensor.sum = Sensor.Left * 1 + Sensor.Middle * 100 + Sensor.Right * 199; // 加权求和
Sensor.location = Sensor.sum / (Sensor.Left + Sensor.Middle + Sensor.Right)-100;      
	// 归一化处理，得到偏差,理论上0表示导线在正中间
}

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式使能
	ADC_InitStructure.ADC_NbrOfChannel = 3;  //这里有四个数据
	ADC_Init(ADC1, &ADC_InitStructure);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//外设源头
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Sensor.ADCConvertedValue;//传入存放的数组名
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 30;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
