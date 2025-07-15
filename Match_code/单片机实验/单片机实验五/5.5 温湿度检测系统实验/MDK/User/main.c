/* 头文件 ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"    
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "jtag_sw.h"
#include "light.h"
#include "led.h"
//硬件外设头文件
#include "oled.h"
#include "dht11.h"
#include "usart2.h"
#include "cloud.h"
#include "IoTMessageHandler.h"
#include "motor.h"
#include "beep.h"
/* 全局变量 ----------------------------------------------------------------*/
char *mymqtt_message;
char *ret;
int retInt;
char temp[20];
char Temp[20];
uint16_t Light_ad;
uint8_t temp_threshold=25;
uint8_t Hum_threshold=70;
uint8_t warning=0;
/* define定义 ------------------------------------------------------------------*/

/* 主函数 ----------------------------------------------------------------*/
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	JTAG_SW_init(1);                                        //设置调试模式为SW模式    
	delay_init();                                           //延时函数时钟初始化 
	BEEP_init();
	unsigned int count = 0;									//定义变量
	OLED_Init();											//OLED灯外设初始化
	OLED_Clear();											//OLED清除显示
	DHT11_init();											//DHT11设初始化
	uart2_init(115200);										//串口初始化
	delay_ms(200);											//延时等待
	Cloud_Init();											//连接华为云初始化
	LIGHT_init();
	MOTOR_init();
	LED_init();
	while(1)
	{
		if(count == 0)
		{
			count = 50;										//降低采集频率和上报频率，放着网络拥堵和降级功耗			
			if(DHT11_ReadData() == 1)
			{
				Light_ad=Read_LIGHT(ADC_Channel_0);
				sprintf(temp,"T:%d H:%d  ",(unsigned int)DHT11.Tem_H,(unsigned int)DHT11.Hum_H);//数据转换成字符串
				OLED_Show_Str(0,0,(unsigned char *)temp);	//显示数据
				sprintf(Temp,"Light=%d  ",(unsigned int)Light_ad);//数据转换成字符串
				OLED_Show_Str(0,2,(unsigned char *)Temp);	//显示数据
				setParamsInt(TopicReport,"TEMP",DHT11.Tem_H);	//向华为云发布消息，值设置温度
				delay_ms(50);
				setParamsInt(TopicReport,"LIGHT",Light_ad);	//向华为云发布消息，值设置温度
				delay_ms(50);
				setParamsInt(TopicReport,"HUM",DHT11.Hum_H);	//向华为云发布消息，值设置湿度
				delay_ms(50);
				setParamsInt(TopicReport,"warning",warning);	//向华为云发布消息，值设置湿度
				delay_ms(50);	
				setParamsInt(TopicReport,"MOTOR",retInt);	//向华为云发布消息，值设置湿度
				delay_ms(50);	
				setParamsInt(TopicReport,"Temp",temp_threshold);	//向华为云发布消息，值设置湿度
				delay_ms(50);	
				setParamsInt(TopicReport,"Hum",Hum_threshold);	//向华为云发布消息，值设置湿度
				delay_ms(50);	
				if(DHT11.Tem_H>temp_threshold||DHT11.Hum_H>temp_threshold){
				BEEP_ON();
				LED1=0;
				warning=1;
				}
				else{
				BEEP_OFF();
				LED1=1;
				warning=0;

				}
			}				
		}
		count --;
		delay_ms(50);
		mymqtt_message =  (char *)atk_esp8266d_uart_rx_get_frame();	//获取串口数据
		if(mymqtt_message !=NULL)							//数据不为空进行数据处理
		{
			if(confirmationMqttMessage(mymqtt_message))		//判断数据是否是订阅数据格式
			{
				retInt = getParamsInt(mymqtt_message,"MOTOR");//数据中查询标识符“MOTOR”的数据值
				temp_threshold=getParamsInt(mymqtt_message,"Temp");
				Hum_threshold=getParamsInt(mymqtt_message,"Hum");
				if(retInt == 0 || retInt == 1 || retInt == 2 )//根据数据值，进行硬件控制 0停止 1正转 2反转
				{
					MOTOR_control(retInt);
				}							
			}
			atk_esp8266d_uart_rx_restart();					//处理后，重新开启串口数据接收功能
		}
		
	}
}


