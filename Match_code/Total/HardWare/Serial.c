#include "HeaderFiles.h"
#include "Serial.h"
uint8_t data_recv=0;
//�����������������
 uint8_t Serial_TxPacket[4];				//FF 01 02 03 04 FE
 uint8_t Serial_RxPacket[4];
 uint8_t Serial_RxValue[4];
 uint8_t Serial_RxFlag;//���ձ�־λ


void  Serial_Init(void){
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART0);
	
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_9|GPIO_PIN_10);//ʹ�ø��ò���ʹ�ô���
	
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_9);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);
	
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_10);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	
	usart_deinit(USART0);//�ȸ�λ����
	usart_baudrate_set(USART0,115200);//���岨����
	usart_parity_config(USART0,USART_PM_NONE);//����У�鷽ʽ
	usart_word_length_set(USART0,USART_WL_8BIT);//���������ֳ�����λ���λ
	usart_stop_bit_set(USART0,USART_STB_1BIT);//����ֹͣλһλ
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);//���յ�ʹ��
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);//ʹ�ܷ���
	
	//�ص�Ӳ�����ؽ��п���
	usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
	
	nvic_irq_enable(USART0_IRQn,0,0);//ʹ�ܶ�Ӧ��NVIC�ж�
	usart_interrupt_enable(USART0,USART_INT_RBNE);
	
	usart_enable(USART0);//ʹ�ܴ���
}

void Serial_SendData(uint16_t *buf,uint16_t len){
	uint16_t t;
	for(t=0;t<len;t++){
	while(usart_flag_get(USART0,USART_FLAG_TC)==RESET);
	usart_data_transmit(USART0,buf[t]);
	}
	while(usart_flag_get(USART0,USART_FLAG_TC)==RESET);
}
//��������
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		usart_data_transmit(USART0,Array[i]);
	}
}

//printfǿ���ض���
int fputc(int ch,FILE *f){
	usart_data_transmit(USART0,(uint8_t)ch);
	while(usart_flag_get(USART0,USART_FLAG_TBE)==RESET);
	return ch;
}
//�������ݰ�����
void Serial_SendPacket(void)
{
	usart_data_transmit(USART0,0xFF);
	Serial_SendArray(Serial_TxPacket, 4);
	usart_data_transmit(USART0,0xFE);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}
//void USART0_IRQHandler(void){
//	if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE)!=RESET)
//	{
//	data_recv=usart_data_receive(USART0);
//	usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
//	}
//}


void USART0_IRQHandler(void)
{
	static uint8_t RxState = 0;//״̬����־
	static uint8_t pRxPacket = 0;//ָʾ���յ�����������
	if (usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE)!=RESET)
	{
		uint8_t RxData = usart_data_receive(USART0);
		
		if (RxState == 0)
		{
			if (RxData == 0xFF)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket ++;
			if (pRxPacket >= 4)
			{
				RxState = 2;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == 0xFE)
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}
		
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
	}
}

	


