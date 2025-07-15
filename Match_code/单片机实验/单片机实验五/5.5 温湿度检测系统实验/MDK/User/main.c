/* ͷ�ļ� ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"    
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "jtag_sw.h"
#include "light.h"
#include "led.h"
//Ӳ������ͷ�ļ�
#include "oled.h"
#include "dht11.h"
#include "usart2.h"
#include "cloud.h"
#include "IoTMessageHandler.h"
#include "motor.h"
#include "beep.h"
/* ȫ�ֱ��� ----------------------------------------------------------------*/
char *mymqtt_message;
char *ret;
int retInt;
char temp[20];
char Temp[20];
uint16_t Light_ad;
uint8_t temp_threshold=25;
uint8_t Hum_threshold=70;
uint8_t warning=0;
/* define���� ------------------------------------------------------------------*/

/* ������ ----------------------------------------------------------------*/
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);         //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	JTAG_SW_init(1);                                        //���õ���ģʽΪSWģʽ    
	delay_init();                                           //��ʱ����ʱ�ӳ�ʼ�� 
	BEEP_init();
	unsigned int count = 0;									//�������
	OLED_Init();											//OLED�������ʼ��
	OLED_Clear();											//OLED�����ʾ
	DHT11_init();											//DHT11���ʼ��
	uart2_init(115200);										//���ڳ�ʼ��
	delay_ms(200);											//��ʱ�ȴ�
	Cloud_Init();											//���ӻ�Ϊ�Ƴ�ʼ��
	LIGHT_init();
	MOTOR_init();
	LED_init();
	while(1)
	{
		if(count == 0)
		{
			count = 50;										//���Ͳɼ�Ƶ�ʺ��ϱ�Ƶ�ʣ���������ӵ�ºͽ�������			
			if(DHT11_ReadData() == 1)
			{
				Light_ad=Read_LIGHT(ADC_Channel_0);
				sprintf(temp,"T:%d H:%d  ",(unsigned int)DHT11.Tem_H,(unsigned int)DHT11.Hum_H);//����ת�����ַ���
				OLED_Show_Str(0,0,(unsigned char *)temp);	//��ʾ����
				sprintf(Temp,"Light=%d  ",(unsigned int)Light_ad);//����ת�����ַ���
				OLED_Show_Str(0,2,(unsigned char *)Temp);	//��ʾ����
				setParamsInt(TopicReport,"TEMP",DHT11.Tem_H);	//��Ϊ�Ʒ�����Ϣ��ֵ�����¶�
				delay_ms(50);
				setParamsInt(TopicReport,"LIGHT",Light_ad);	//��Ϊ�Ʒ�����Ϣ��ֵ�����¶�
				delay_ms(50);
				setParamsInt(TopicReport,"HUM",DHT11.Hum_H);	//��Ϊ�Ʒ�����Ϣ��ֵ����ʪ��
				delay_ms(50);
				setParamsInt(TopicReport,"warning",warning);	//��Ϊ�Ʒ�����Ϣ��ֵ����ʪ��
				delay_ms(50);	
				setParamsInt(TopicReport,"MOTOR",retInt);	//��Ϊ�Ʒ�����Ϣ��ֵ����ʪ��
				delay_ms(50);	
				setParamsInt(TopicReport,"Temp",temp_threshold);	//��Ϊ�Ʒ�����Ϣ��ֵ����ʪ��
				delay_ms(50);	
				setParamsInt(TopicReport,"Hum",Hum_threshold);	//��Ϊ�Ʒ�����Ϣ��ֵ����ʪ��
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
		mymqtt_message =  (char *)atk_esp8266d_uart_rx_get_frame();	//��ȡ��������
		if(mymqtt_message !=NULL)							//���ݲ�Ϊ�ս������ݴ���
		{
			if(confirmationMqttMessage(mymqtt_message))		//�ж������Ƿ��Ƕ������ݸ�ʽ
			{
				retInt = getParamsInt(mymqtt_message,"MOTOR");//�����в�ѯ��ʶ����MOTOR��������ֵ
				temp_threshold=getParamsInt(mymqtt_message,"Temp");
				Hum_threshold=getParamsInt(mymqtt_message,"Hum");
				if(retInt == 0 || retInt == 1 || retInt == 2 )//��������ֵ������Ӳ������ 0ֹͣ 1��ת 2��ת
				{
					MOTOR_control(retInt);
				}							
			}
			atk_esp8266d_uart_rx_restart();					//��������¿����������ݽ��չ���
		}
		
	}
}


