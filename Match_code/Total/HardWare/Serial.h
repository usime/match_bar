#ifndef __SERIAL_H
#define __SERIAL_H
#include "HeaderFiles.h"
//�����������������
extern uint8_t Serial_TxPacket[4];				//FF 01 02 03 04 FE
extern uint8_t Serial_RxPacket[4];
extern uint8_t Serial_RxValue[4];
extern uint8_t Serial_RxFlag;//���ձ�־λ
void  Serial_Init(void);
uint8_t Serial_GetRxFlag(void);
void Serial_SendPacket(void);


#endif


