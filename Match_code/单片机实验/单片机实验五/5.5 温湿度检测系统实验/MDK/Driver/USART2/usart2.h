#ifndef __USART2_H
#define __USART2_H
#include "stm32f10x.h"
#include "stdint.h"
#include "stdio.h"

#define USART_REC_LEN2 200
extern u8  USART_RX_BUF2[USART_REC_LEN2]; //接收缓冲,最大USART_REC_LEN2个字节.末字节为换行符 
extern u16 USART_RX_STA2;         			//接收状态标记	

void uart2_init(u32 bound);
void USART2_IRQHandler(void) ;               	//串口2中断服务程序


#endif


