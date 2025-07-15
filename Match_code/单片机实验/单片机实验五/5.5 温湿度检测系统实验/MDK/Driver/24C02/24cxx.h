#ifndef __24CXX_H
#define __24CXX_H
#include <stm32f10x.h>
#include "sys.h"


//-----------------OLED IIC端口定义----------------  
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
//使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02


//IIC所有操作函数
void IIC_Init(void);                															//初始化IIC的IO口				 
void IIC_Start(void);																			//发送IIC开始信号
void IIC_Stop(void);	  																		//发送IIC停止信号
void IIC_Send_Byte(unsigned char txd);															//IIC发送一个字节
unsigned char IIC_Read_Byte(unsigned char ack);													//IIC读取一个字节
unsigned char IIC_Wait_Ack(void); 																//IIC等待ACK信号
void IIC_Ack(void);																				//IIC发送ACK信号
void IIC_NAck(void);																			//IIC不发送ACK信号

void IIC_Write_One_Byte(unsigned char daddr,unsigned char addr,unsigned char data);
unsigned char IIC_Read_One_Byte(unsigned char daddr,unsigned char addr);	  

					  
unsigned char AT24CXX_ReadOneByte(unsigned int ReadAddr);										//指定地址读取一个字节
void AT24CXX_WriteOneByte(unsigned int WriteAddr,unsigned char DataToWrite);					//指定地址写入一个字节
void AT24CXX_WriteLenByte(unsigned int WriteAddr,unsigned long DataToWrite,unsigned char Len);	//指定地址开始写入指定长度的数据
unsigned long AT24CXX_ReadLenByte(unsigned int ReadAddr,unsigned char Len);						//指定地址开始读取指定长度数据
void AT24CXX_Write(unsigned int WriteAddr,unsigned char *pBuffer,unsigned int NumToWrite);		//从指定地址开始写入指定长度的数据
void AT24CXX_Read(unsigned int ReadAddr,unsigned char *pBuffer,unsigned int NumToRead);   		//从指定地址开始读出指定长度的数据


unsigned char AT24CXX_Check(void); 																//检查器件
void AT24CXX_Init(void); 																		//初始化IIC
#endif
















