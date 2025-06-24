#ifndef __Usart_H
#define __Usart_H
#include "stm32f10x.h"
#include "sys.h"
#include "stdio.h"
void Usart1_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint16_t *Array, uint16_t Length);
void Serial_SendString(char *String);
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
int fputc(int ch, FILE *f);
void Serial_Printf(char *format, ...);
#endif 
