#include "HeaderFiles.h"
#include "Serial.h"
// ״̬��״̬����
#define STATE_WAITING_START  0    // �ȴ����տ�ʼ״̬
#define STATE_RECEIVING      1    // ���ڽ�������״̬
#define STATE_RECEIVED_CR    2    // ���յ��س���״̬
//uint8_t data_recv=0;
char data_receive[256];
volatile uint8_t data_ready = 0;
uint8_t data_receive_index=0;
uint8_t data_receive_flag=0; 
uint8_t pRxPacket = 0;
uint8_t usart_state=0;
//�����������������
 uint8_t Serial_TxPacket[4];				//FF 01 02 03 04 FE
 uint8_t Serial_RxPacket[4];
 uint8_t Serial_RxValue[4];
 uint8_t Serial_RxFlag;//���ձ�־λ
 uint8_t RxState=0;
ParameterConfig Par_Init;
void Parm_Init(ParameterConfig *Par_Init){
Par_Init->Key1_ret_val=20;
Par_Init->Key2_ret_val=30;
Par_Init->Key3_ret_val=40;
Par_Init->Key4_ret_val=50;
Par_Init->test_ret_val=1;
Par_Init->date_num_ret_val=2;
Par_Init->config_read_ret_val=3;
Par_Init->config_save_ret_val=4;
Par_Init->conf_ret_val=5;
Par_Init->RTC_Config_ret_val=6;
Par_Init->RTC_NOW_ret_val=7;
Par_Init->limit_ret_val=8;
Par_Init->ratio_ret_val=9;
Par_Init->start_ret_val=10;
Par_Init->stop_ret_val=11;
Par_Init->unhide_ret_val=12;
Par_Init->hide_ret_val=13;
}
void  Serial_Init(void){
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART0);
	
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_9|GPIO_PIN_10);//ʹ�ø��ò���ʹ�ô���
	
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_9);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_10);
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
    //���ڵĽ����ж�  
	nvic_irq_enable(USART0_IRQn,0,0);//ʹ�ܶ�Ӧ��NVIC�ж�
	usart_interrupt_enable(USART0,USART_INT_RBNE);
	usart_enable(USART0);//ʹ�ܴ���
}


void gd_eval_com_init(void)
{

/* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* configure the USART0 TX pin and USART0 RX pin */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

    /* configure USART0 TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* configure USART0 RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);

}


void Serial_SendData(uint8_t Byte){
	usart_data_transmit(USART0,Byte);
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

uint8_t Usart_flag_ret(char *Tx_buffer,uint8_t Keynum,ParameterConfig *Par_Init){
if(strlen((char*)Tx_buffer)==4&&Keynum==0&&data_receive_flag==1){
		if(strcmp(Tx_buffer,"test")==0){
				return Par_Init->test_ret_val;
		}
		else if(strcmp(Tx_buffer,"conf")==0){
				return Par_Init->conf_ret_val;
		}
		else if(strcmp(Tx_buffer,"hide")==0){
				return Par_Init->hide_ret_val;
		}
		else if(strcmp(Tx_buffer,"stop")==0){
				return Par_Init->stop_ret_val;
		}
}
else if(strlen((char*)Tx_buffer)==5&&Keynum==0){
		if(strcmp(Tx_buffer,"start")==0){
				return Par_Init->start_ret_val;
		}
		else if(strcmp(Tx_buffer,"limit")==0){
				return Par_Init->limit_ret_val;
		}
		else if(strcmp(Tx_buffer,"ratio")==0){
				return Par_Init->ratio_ret_val;
		}
}	
else if(strlen((char*)Tx_buffer)==6&&Keynum==0){
		if(strcmp(Tx_buffer,"unhide")==0){
				return Par_Init->unhide_ret_val;
		}
}
else if(strlen((char*)Tx_buffer)==7&&Keynum==0){
		if(strcmp(Tx_buffer,"RTC now")==0){
				return Par_Init->RTC_NOW_ret_val;
		}
}
else if(strlen((char*)Tx_buffer)==10&&Keynum==0){
		if(strcmp(Tx_buffer,"RTC Config")==0){
				return Par_Init->RTC_Config_ret_val;
		}
}
else if(strlen((char*)Tx_buffer)==11&&Keynum==0){
		if(strcmp(Tx_buffer,"config save")==0){
				return Par_Init->config_save_ret_val;
		}
		else if(strcmp(Tx_buffer,"config read")==0){
				return Par_Init->config_read_ret_val;
		}
}
else if(strlen((char*)Tx_buffer)==19&&Keynum==0){
				return Par_Init->date_num_ret_val;
}
else if(Keynum==1){
return Par_Init->Key1_ret_val;
}
else if(Keynum==2){
return Par_Init->Key2_ret_val;
}
else if(Keynum==3){
return Par_Init->Key3_ret_val;
}
else if(Keynum==4){
return Par_Init->Key4_ret_val;
}
}






//����ʹ�õĽ��ܵ��ֽڲ����뻺���������жϺ���
void USART0_IRQHandler(void){
    if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET) {
        // �������ݴ��뻺����

		   uint8_t rx_data = usart_data_receive(USART0);

        switch (usart_state) {
            case STATE_WAITING_START: // �ȴ����տ�ʼ״̬
                data_receive[data_receive_index] = rx_data;
                data_receive_index++;
                usart_state = STATE_RECEIVING;
                break;

            case STATE_RECEIVING: // ���ڽ�������״̬
                if (rx_data == '\r') {
                    usart_state = STATE_RECEIVED_CR;
                } else if (rx_data == '\n') {
                    // ���ֱ�ӽ��յ� '\n'������ǰ��û�� '\r'��Ҳ��Ϊ�������
                    data_receive[data_receive_index] = '\0'; // ����ַ���������
                    data_receive_flag = 1;                   // ���ý�����ɱ�־
                    usart_state = STATE_WAITING_START;       // ����״̬��
                } else {
                    if (data_receive_index < 256 - 1) {
                        data_receive[data_receive_index] = rx_data;
                        data_receive_index++;
                    }
                }
                break;
            case STATE_RECEIVED_CR: // ���յ��س���״̬
                if (rx_data == '\n') {
                    data_receive[data_receive_index] = '\0'; // ����ַ���������
                    data_receive_flag = 1;                   // ���ý�����ɱ�־
                    usart_state = STATE_WAITING_START;       // ����״̬��
                } else {
                    // ����� '\r' ֮��û���յ� '\n'��������������
                    if (data_receive_index < 256 - 1) {
                        data_receive[data_receive_index - 1] = rx_data; // ���� '\r'
                        data_receive_index++;
                    }
                    usart_state = STATE_RECEIVING;
                }
                break;

            default:
                usart_state = STATE_WAITING_START;
                break;
        }

        // ��黺�����Ƿ����
        if (data_receive_index >= 255) {
            data_receive_index = 0;  // ��ѡ����ջ��������ȡ���������ʩ
        }
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
    }
}



//void USART0_IRQHandler(void){
//	if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE)!=RESET)
//	{
//	data_recv=usart_data_receive(USART0);
//	usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
//	}
//}


//��������жϺ�����������HEX���ݰ�
//void USART0_IRQHandler(void)
//{
//	static uint8_t RxState = 0;//״̬����־
//	static uint8_t pRxPacket = 0;//ָʾ���յ�����������
//	if (usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE)!=RESET)
//	{
//		uint8_t RxData = usart_data_receive(USART0);
//		
//		if (RxState == 0)
//		{
//			if (RxData == 0xFF)
//			{
//				RxState = 1;
//				pRxPacket = 0;
//			}
//		}
//		else if (RxState == 1)
//		{
//			Serial_RxPacket[pRxPacket] = RxData;
//			pRxPacket ++;
//			if (pRxPacket >= 4)
//			{
//				RxState = 2;
//			}
//		}
//		else if (RxState == 2)
//		{
//			if (RxData == 0xFE)
//			{
//				RxState = 0;
//				Serial_RxFlag = 1;
//			}
//		}
//		
//		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
//	}
//}

	


