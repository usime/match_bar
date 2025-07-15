#ifndef __24CXX_H
#define __24CXX_H
#include <stm32f10x.h>
#include "sys.h"


//-----------------OLED IIC�˿ڶ���----------------  
#define 	RCC_AHBxPeriph_IIC_SCK    	RCC_APB2Periph_GPIOB
#define 	IIC_SCK_GPIO_PORT      		GPIOB
#define  	IIC_SCK_GPIO_PIN   			GPIO_Pin_6
#define 	IIC_SCK						PBout(6)

#define 	RCC_AHBxPeriph_IIC_SDA    	RCC_APB2Periph_GPIOB
#define  	IIC_SDA_GPIO_PORT			GPIOB
#define  	IIC_SDA_GPIO_PIN   			GPIO_Pin_10
#define		IIC_SDA_OUT					PBout(10)
#define		IIC_SDA_IN					PBin(10)


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02
#define EE_TYPE AT24C02


//IIC���в�������
void IIC_Init(void);                															//��ʼ��IIC��IO��				 
void IIC_Start(void);																			//����IIC��ʼ�ź�
void IIC_Stop(void);	  																		//����IICֹͣ�ź�
void IIC_Send_Byte(unsigned char txd);															//IIC����һ���ֽ�
unsigned char IIC_Read_Byte(unsigned char ack);													//IIC��ȡһ���ֽ�
unsigned char IIC_Wait_Ack(void); 																//IIC�ȴ�ACK�ź�
void IIC_Ack(void);																				//IIC����ACK�ź�
void IIC_NAck(void);																			//IIC������ACK�ź�

void IIC_Write_One_Byte(unsigned char daddr,unsigned char addr,unsigned char data);
unsigned char IIC_Read_One_Byte(unsigned char daddr,unsigned char addr);	  

					  
unsigned char AT24CXX_ReadOneByte(unsigned int ReadAddr);										//ָ����ַ��ȡһ���ֽ�
void AT24CXX_WriteOneByte(unsigned int WriteAddr,unsigned char DataToWrite);					//ָ����ַд��һ���ֽ�
void AT24CXX_WriteLenByte(unsigned int WriteAddr,unsigned long DataToWrite,unsigned char Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
unsigned long AT24CXX_ReadLenByte(unsigned int ReadAddr,unsigned char Len);						//ָ����ַ��ʼ��ȡָ����������
void AT24CXX_Write(unsigned int WriteAddr,unsigned char *pBuffer,unsigned int NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void AT24CXX_Read(unsigned int ReadAddr,unsigned char *pBuffer,unsigned int NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����


unsigned char AT24CXX_Check(void); 																//�������
void AT24CXX_Init(void); 																		//��ʼ��IIC
#endif
















