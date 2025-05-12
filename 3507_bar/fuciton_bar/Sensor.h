#ifndef __SENSOR_H
typedef struct
{
uint16_t ADCConvertedValue[10][3];
int ADC_filter_Value[3];
int Left,Middle,Right;
int sum;
int location;
}Sensor_Data;
void ADC_fliter(void);
//通过此函数归一化处理得到小车的位置	
void circulate_location(void);
void AD_Init(void);

#define __SENSOR_H
#endif


