#ifndef __USART_H__
#define __USART_H__

#include "main.h"
//���ڷ��͵����ַ�
void uart0_send_char(char ch);
//���ڷ����ַ���
void uart0_send_string(char* str);
void usart0_send_byte(unsigned char byte);
void usart0_send_bytes(unsigned char *buf, int len);
int fputc(int ch, FILE *f);

void usart1_Init(void);
void JustFloat_SendArray(uint8_t *string,uint8_t length);
/*��������fת��Ϊ4���ֽ����ݴ����byte[4]��*/
void Float_to_Byte(float f,unsigned char byte[]);
//justfloat ����Э�����
void JustFloat_Test(void);	
//��vofa��������  ��������  ����ͨ��  ���ӻ���ʾ  ֡β
void vofa_sendData(float a,float b,float c);


uint8_t HAL_UART_Transmit( uint8_t *pData, uint16_t Size, uint32_t Timeout);















#endif
