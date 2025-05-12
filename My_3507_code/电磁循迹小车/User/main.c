#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "Motor.h"
#include "Sensor.h"
#include "Timer.h"
extern Sensor_Data Sensor;
extern int cnt;
extern int L_Duty;
extern int R_Duty;
//如果占空比基础值为2500，则Kp为25左右即可循迹
#define basic 4000
float Kp=32, 
	  Ki=0,
	  Kd=105;


int main(void)
{
	OLED_Init();
	PWM_Init();
	Motor_Pin_Init();
	Motor_Init();
	AD_Init();
	Timer_Init();

	while (1)
	{
//		SetDuty(30,30);//电机测试
		OLED_ShowNum(1,1,Sensor.ADC_filter_Value[0],3);
		OLED_ShowNum(1,6,Sensor.ADC_filter_Value[1],3);
		OLED_ShowNum(1,12,Sensor.ADC_filter_Value[2],3);
		OLED_ShowChar(2,1,'L');
		OLED_ShowSignedNum(2,3,basic+L_Duty,4);
		OLED_ShowChar(2,9,'R');
		OLED_ShowSignedNum(2,11,basic-R_Duty,4);
		OLED_ShowSignedNum(3,1,L_Duty,4);
		OLED_ShowSignedNum(4,1,Sensor.location,4);
//		OLED_ShowNum(4,1,cnt,4);
//		delay_ms(100);
	}
}	

