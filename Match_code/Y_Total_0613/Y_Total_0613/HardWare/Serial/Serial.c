#include "HeaderFiles.h"
#include "Serial.h"
// 状态机状态定义
#define STATE_WAITING_START  0    // 等待接收开始状态
#define STATE_RECEIVING      1    // 正在接收数据状态
#define STATE_RECEIVED_CR    2    // 接收到回车符状态
//uint8_t data_recv=0;
char data_receive[256];
volatile uint8_t data_ready = 0;
uint8_t data_receive_index=0;
uint8_t data_receive_flag=0; 
uint8_t pRxPacket = 0;
uint8_t usart_state=0;
//这是两个缓存的数据
 uint8_t Serial_TxPacket[4];				//FF 01 02 03 04 FE
 uint8_t Serial_RxPacket[4];
 uint8_t Serial_RxValue[4];
 uint8_t Serial_RxFlag;//接收标志位
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
	
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_9|GPIO_PIN_10);//使用复用才能使用串口
	
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_9);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_10);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	usart_deinit(USART0);//先复位串口
	usart_baudrate_set(USART0,115200);//定义波特率
	usart_parity_config(USART0,USART_PM_NONE);//定义校验方式
	usart_word_length_set(USART0,USART_WL_8BIT);//定义数据字长，八位或九位
	usart_stop_bit_set(USART0,USART_STB_1BIT);//定义停止位一位
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);//接收的使能
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);//使能发送
	//关掉硬件流控进行控制
	usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
    //串口的接收中断  
	nvic_irq_enable(USART0_IRQn,0,0);//使能对应的NVIC中断
	usart_interrupt_enable(USART0,USART_INT_RBNE);
	usart_enable(USART0);//使能串口
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
//发送数组
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		usart_data_transmit(USART0,Array[i]);
	}
}

//printf强制重定义
int fputc(int ch,FILE *f){
	usart_data_transmit(USART0,(uint8_t)ch);
	while(usart_flag_get(USART0,USART_FLAG_TBE)==RESET);
	return ch;
}
//发送数据包内容
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






//正常使用的接受单字节并存入缓冲区串口中断函数
void USART0_IRQHandler(void){
    if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET) {
        // 接收数据存入缓冲区

		   uint8_t rx_data = usart_data_receive(USART0);

        switch (usart_state) {
            case STATE_WAITING_START: // 等待接收开始状态
                data_receive[data_receive_index] = rx_data;
                data_receive_index++;
                usart_state = STATE_RECEIVING;
                break;

            case STATE_RECEIVING: // 正在接收数据状态
                if (rx_data == '\r') {
                    usart_state = STATE_RECEIVED_CR;
                } else if (rx_data == '\n') {
                    // 如果直接接收到 '\n'，可能前面没有 '\r'，也认为接收完成
                    data_receive[data_receive_index] = '\0'; // 添加字符串结束符
                    data_receive_flag = 1;                   // 设置接收完成标志
                    usart_state = STATE_WAITING_START;       // 重置状态机
                } else {
                    if (data_receive_index < 256 - 1) {
                        data_receive[data_receive_index] = rx_data;
                        data_receive_index++;
                    }
                }
                break;
            case STATE_RECEIVED_CR: // 接收到回车符状态
                if (rx_data == '\n') {
                    data_receive[data_receive_index] = '\0'; // 添加字符串结束符
                    data_receive_flag = 1;                   // 设置接收完成标志
                    usart_state = STATE_WAITING_START;       // 重置状态机
                } else {
                    // 如果在 '\r' 之后没有收到 '\n'，继续接收数据
                    if (data_receive_index < 256 - 1) {
                        data_receive[data_receive_index - 1] = rx_data; // 覆盖 '\r'
                        data_receive_index++;
                    }
                    usart_state = STATE_RECEIVING;
                }
                break;

            default:
                usart_state = STATE_WAITING_START;
                break;
        }

        // 检查缓冲区是否溢出
        if (data_receive_index >= 255) {
            data_receive_index = 0;  // 可选择清空缓冲区或采取其他处理措施
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


//这个串口中断函数用来接收HEX数据包
//void USART0_IRQHandler(void)
//{
//	static uint8_t RxState = 0;//状态机标志
//	static uint8_t pRxPacket = 0;//指示接收到几个数据了
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

	


