#include "stm32f10x.h"               
#include "Delay.h"
#include "OLED.h"
#include "stdio.h"
#include "multibutton.h"
#include "SR03.h"  			
#include "Timer.h"
#include "HT11.h"
#include "JTAG.h"
#include "Key.h"
#include "Beep.h"
#include "OLED_Menu.h"
#include "LED.h"
#include "seg.h"
extern int cnt;
extern int Distance;
extern Button btn3;
extern Button btn1;
extern Button btn2;
uint8_t Key_num=0;
extern uint8_t Key_num;
extern int Distance;
extern float Pitch,Roll,Yaw;
float speed;
uint8_t flag=1;
int fiter_dist=0;
int median_Dist_buffer[50]={0};
int moving_Dist_buffer[50]={0};
char Temp_buffer[50]={0};//温度
char Tum_buffer[50]={0};//湿度
char Uart_buffer[100];
int OLED_Flag=0;
int moving_buffer_index = 0;
int median_buffer_index = 0;
int update_time=0;
int last_update_time=0;
int update_time1=0;
int last_update_time1=0;
extern uint16_t CNT ;
int Key_flag=0;
int moving_filter_dist=0;
int actual_dist=0;
int median_filter_dist=0;
void BTN1_SINGLEClick_Handler(void* btn)
{
	Key_num--;
	if(Key_num==255||Key_num==0){
	Key_num=4;
	}
}

void BTN2_SINGLEClick_Handler(void* btn)
{
	Key_num++; 
		if(Key_num==5){
	Key_num=1;
		}
}
void BTN3_SINGLEClick_Handler(void* btn)
{
	OLED_Clear(); 
}
int main(void)
{
	SEG_init();
	LED_Init();
	OLED_Init();
	JTAG_SW_init(1);        										
	OLED_Clear();	
	Key_3_Init();
	Beep_Init();
	Key_1_2_Init();
	button_init(&btn1, read_button_GPIO, 0, btn1_id);
	button_init(&btn2, read_button_GPIO, 0, btn2_id);
	button_init(&btn3, read_button_GPIO, 0, btn3_id);
	button_attach(&btn1, SINGLE_CLICK,BTN1_SINGLEClick_Handler);
	button_attach(&btn2, SINGLE_CLICK,BTN2_SINGLEClick_Handler);
	button_attach(&btn3, SINGLE_CLICK,BTN3_SINGLEClick_Handler);
	button_start(&btn1);
	button_start(&btn2);
	button_start(&btn3);
	DHT11_init();	
	SR03_Init();
	Timer_Init(7200,1000);//每100ms一次中断
	Timer2_init(7200,50);//每5ms一次中断
	DHT11_ReadData();	
	while (1){	

	speed=331.5+0.6*(DHT11.Tem_H+DHT11.Tem_L/10);
	update_time=CNT;
	update_time1=CNT;
	if((update_time-last_update_time)>=2000){
	last_update_time=update_time;
	DHT11_ReadData();	
	}
	median_filter_dist=median_filter(Distance,median_Dist_buffer,&median_buffer_index);
	moving_filter_dist=moving_filter(median_filter_dist,moving_Dist_buffer,&moving_buffer_index);
	if((update_time1-last_update_time1)>=10){
	last_update_time1=update_time1;
	SR03_Strat();
	light_dist(Distance/1000);
	actual_dist=linear_compensation_function(moving_filter_dist);
	Set_Beep_compare(Dist_tocompare(Distance/1000));
	}
	switch(Key_num){
				case 1:
		Write_Seg(DHT11.Tem_H/10, 0);  
		Write_Seg(DHT11.Tem_H%10, 1);  
		Write_Seg(DHT11.Tem_L, 2);  
    break;
				case 2:
		Write_Seg(actual_dist/100000, 0); 
		Write_Seg(((actual_dist/1000)%100)/10, 1); 
		Write_Seg(((actual_dist/1000)%10),2); 
		Write_Seg(((actual_dist/1000)%10),3); 
    break;
				case 3:
		Write_Seg(speed/100, 0);  //
		Write_Seg((((int)speed/10)%10), 1);  // 
		Write_Seg((((int)speed/10)%30), 2);		//
    break;
				case 4:
		sprintf(Temp_buffer, "Temp=%d.%d ",DHT11.Tem_H,DHT11.Tem_L/10 );
		sprintf(Uart_buffer, "dist=%d.%02d ", actual_dist/1000,actual_dist%100);
		OLED_ShowString(1,3,Temp_buffer,10);
		OLED_ShowString(1,1,Uart_buffer,10);
    break;
	}
}
		return 0;
	}



 
//char DHTdata1[5];
 
 
 
		

