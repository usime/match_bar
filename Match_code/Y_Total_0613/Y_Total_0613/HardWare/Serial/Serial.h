#ifndef __SERIAL_H
#define __SERIAL_H
#include "HeaderFiles.h"
//这是两个缓存的数据
//extern uint8_t data_recv;

typedef struct{
uint8_t Key1_ret_val;
uint8_t Key2_ret_val;
uint8_t Key3_ret_val;	
uint8_t Key4_ret_val;	
uint8_t test_ret_val;   
uint8_t RTC_Config_ret_val;
uint8_t date_num_ret_val;
uint8_t RTC_NOW_ret_val; 
uint8_t conf_ret_val; 
uint8_t ratio_ret_val; 
uint8_t start_ret_val; 
uint8_t stop_ret_val; 
uint8_t config_save_ret_val; 
uint8_t config_read_ret_val; 
uint8_t limit_ret_val; 
uint8_t hide_ret_val; 
uint8_t unhide_ret_val;
}ParameterConfig;

extern ParameterConfig Par_Init;
extern uint8_t data_receive_index;//缓冲区索引
extern char data_receive[256];
extern uint8_t usart_state;
extern uint8_t data_receive_flag;
extern volatile uint8_t data_ready ;
extern uint8_t Serial_TxPacket[4];				//FF 01 02 03 04 FE
extern uint8_t Serial_RxPacket[4];
extern uint8_t Serial_RxValue[4];
extern uint8_t Serial_RxFlag;//接收标志位
void  Serial_Init(void);
void gd_eval_com_init(void);
uint8_t Serial_GetRxFlag(void);
void Serial_SendPacket(void);
void Serial_SendData(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
uint8_t Usart_flag_ret(char *Tx_buffer,uint8_t Keynum,ParameterConfig *Par_Init);

#endif


