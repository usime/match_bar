#ifndef __USART2_H
#define __USART2_H
#include "stm32f10x.h"
#include "stdint.h"
#include "stdio.h"

#define USART_REC_LEN2 200
extern u8  USART_RX_BUF2[USART_REC_LEN2]; //���ջ���,���USART_REC_LEN2���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA2;         			//����״̬���	

void uart2_init(u32 bound);
void USART2_IRQHandler(void) ;               	//����2�жϷ������


#endif


