#include "stm32f10x.h"               
#include "Delay.h"
#include "OLED.h"
#include "stdio.h"
#include "multibutton.h"			
#include "Timer.h"
#include "HT11.h"
#include "JTAG.h"
#include "Key.h"
#include "Beep.h"
#include "OLED_Menu.h"
#include "LED.h"
#include "seg.h"
#include "DMA.h"
#include "motor.h"
#include "SR03.h"
#include <cmath>
#include "Usart.h"
#include "filter.h"
#include "Hong Wai.h"
#include "PID.h"
extern uint16_t AD_Value[4];
extern uint16_t CNT;
extern int Distance;
extern Button btn3;
extern Button btn1;
extern Button btn2;
extern uint16_t Usart_Buffer[100];
int Key_num=0;
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
char Uart1_buffer[100];
char Uart2_buffer[100];
char Uart3_buffer[100];
int OLED_Flag=0;
int AD_Float;
int ad_result=0;
int moving_buffer_index = 0;
int median_buffer_index = 0;
int update_time=0;
int last_update_time=0;
int update_time1=0;
int last_update_time1=0;
extern uint16_t CNT ;
int Key_flag=0;
int AD_Result=0;
int moving_filter_dist=0;
int actual_dist=0;
float AD_Result_Kerman=0.0f;
int Set_speed=0;
int median_filter_dist=0;
float last_update_adc=0.0f;
uint16_t Pulse_CNT=0;
uint8_t Turn_mode_flag=1;
int Pulse_Cnt=0;
extern int Speed;
extern uint32_t Current_pulse;
extern uint32_t Last_Pulse_CNT;
extern uint32_t pulse_diff;
extern float actual_speed1;
extern uint32_t fliter_value;
extern int filter_speed;
extern uint32_t Actual_speed;
uint32_t Set_speed2;
void BTN1_LongClick_Handler(void* btn)
{
	Key_num=Key_num+5; 
}

void BTN1_SingleClick_Handler(void* btn)
{
	Key_num=Key_num+1; 
}
void BTN1_DoubleClick_Handler(void* btn)
{
	Current_State=Forward;
}
void BTN2_LongClick_Handler(void* btn)
{
	Key_num=Key_num-5;

}

void BTN2_SingleClick_Handler(void* btn)
{
		Key_num=Key_num-1;

}

void BTN2_DoubleClick_Handler(void* btn)
{
	Current_State=Back;
}
void BTN3_SINGLEClick_Handler(void* btn)
{
	Turn_mode_flag++;
	Turn_mode_flag=Turn_mode_flag%2;//调制模式开始
}
int main(void)
{
	OLED_Init();     										
	OLED_Clear();	
	//JTAG_SW_init(1);
	Key_3_Init();
	Key_1_2_Init();
	button_init(&btn1, read_button_GPIO, 0, btn1_id);
	button_init(&btn2, read_button_GPIO, 0, btn2_id);
	button_init(&btn3, read_button_GPIO, 0, btn3_id);
	button_attach(&btn1, LONG_PRESS_HOLD,BTN1_LongClick_Handler);
	button_attach(&btn2, LONG_PRESS_HOLD,BTN2_LongClick_Handler);
	Beep_Init();
	Motor_En_Init();
	TIM3_PWM_Init();
	
	
	PID_Speed_Init(&Motor_Ctrl);
	button_attach(&btn1, SINGLE_CLICK,BTN1_SingleClick_Handler);
	button_attach(&btn2, SINGLE_CLICK,BTN2_SingleClick_Handler);
	button_attach(&btn3, SINGLE_CLICK,BTN3_SINGLEClick_Handler);
	
	
	button_attach(&btn1, DOUBLE_CLICK,BTN1_DoubleClick_Handler);
	button_attach(&btn2, DOUBLE_CLICK,BTN2_DoubleClick_Handler);
	button_start(&btn1);
	button_start(&btn2);
	button_start(&btn3);
	USART2_Init();

	Light_Init();
	Timer2_init(7200,50);//50ms采样一次
	EXTi_Init();
	PID_Speed_SetK(&Motor_Ctrl,2.4,0.012,0.3);
	Set_Motor_Enable();
	
	while (1){
		if((AD_Value[0]+Key_num)/2>1000){
		Set_speed2=(AD_Value[0]+Key_num)/2;
		}
		else{
		Set_speed2=2000-(AD_Value[0]+Key_num)/2;
		}
		printf("%d,%d\n",Actual_speed,Set_speed2);
		sprintf(Uart_buffer,"Adjust_mode=%d",Turn_mode_flag);
		OLED_ShowString(1,0,Uart_buffer,30);	
		sprintf(Uart1_buffer,"set_speed=%d",Set_speed2);
		OLED_ShowString(1,2,Uart1_buffer,30);	
		sprintf(Uart2_buffer,"now_speed=%d",Actual_speed);
		OLED_ShowString(1,4,Uart2_buffer,30);
			if(Turn_mode_flag==0){
				if((AD_Value[0]+Key_num)/2>1000){
				Load_Motor_pwm(Actual_speed,0);
					if((AD_Value[0]+Key_num)/2>1300){
					Set_Beep_compare(8000);
					}
					else{
					Set_Beep_compare(8000);
					}
				}
				else if((AD_Value[0]+Key_num)/2<1000){
				Load_Motor_pwm(0,Actual_speed);
					if((AD_Value[0]+Key_num)/2<700){
					Set_Beep_compare(8000);
					}
					else{
					Set_Beep_compare(0);
					}
				}
			}else{
			Set_Target_speed(&Motor_Ctrl,Set_speed2);
			Load_Motor_pwm(0,0);
			}	
}
	return 0;
}
