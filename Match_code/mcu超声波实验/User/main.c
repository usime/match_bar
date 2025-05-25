#include "stm32f10x.h"               
#include "Delay.h"
#include "OLED.h"
#include "stdio.h"
#include "multibutton.h"
#include "Key.h"
#include "SR03.h"  			
#include "Timer.h"
#include "HT11.h"
#include "JTAG.h"
extern int cnt;
extern int Distance;
int filter_dist=0;
int Dist_buffer[100]={0};
char Temp_buffer[50]={0};//温度
char Tum_buffer[50]={0};//湿度
char Uart_buffer[100];
int buffer_index = 0;
int update_time=0;
int last_update_time=0;
extern int CNT;
float speed;
int main(void)
{
	OLED_Init();
	JTAG_SW_init(1);        
	OLED_Init();											
	OLED_Clear();											
	DHT11_init();	
	SR03_Init();
	Timer_Init(7200,1000);
	Timer2_init(7200,1000);
	DHT11_ReadData();	
	while (1)
{
	speed=331.5+0.6*(DHT11.Tem_H+DHT11.Tem_L/10);
	update_time=CNT;
	if((update_time-last_update_time)>=100){
	last_update_time=update_time;
	OLED_ShowNum(5,5,CNT,3,10);		
	DHT11_ReadData();	
	}
	filter_dist=moving_filter(Distance,Dist_buffer,&buffer_index);
	sprintf(Temp_buffer, "Temp=%d.%01d ", DHT11.Tem_H, DHT11.Tem_L);
	sprintf(Uart_buffer, "dist=%d.%02d ", Distance/1000, Distance%100);
	OLED_ShowString(1,3,Temp_buffer,10);
	OLED_ShowString(1,1,Uart_buffer,10);
	SR03_Strat();
//	DHT11_Read_Data(&tem1,&hum1);
//	sprintf(Temp,"TEMP%d",tem1);
//	sprintf(Tum,"TUM%d",hum1);
////
	//Delay_ms(500);
}
	return 0;
}

 
//char DHTdata1[5];
 
 
 
		

