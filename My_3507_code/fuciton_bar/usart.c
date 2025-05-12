/***
	*******************************************************************************************************************************************************************
	* @file    usart.c
	* @version V2.1
	* @date    2024-7-22
	* @author  ����	
	* @brief   MSPM0G3507С��PID����ͨ��ģ��
   *************************************************************************************************
   *  @description
	*	
	*  �ӿ����ÿ���ʹ��Sysconfig��
   *
>>>>> ����˵����δ������������ת�����������״��룬��Ҷ����ҹ�����Ϊ���꣬�뱣�ֺ��Լ��ĳ��ģ��ڴˣ��������ҵĸ�л
	*************************************************************************************************************************************************************
***/

#include "usart.h"
#include "protocol.h"
void usart1_Init(void)
{
	// SYSCFG???
	SYSCFG_DL_init();
	//????????
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	//??????
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

}

//���ڷ��͵����ַ�
void uart0_send_char(char ch)
{
   
    while(DL_UART_isBusy(UART_0_INST) == true);
    DL_UART_Main_transmitData(UART_0_INST, ch);
}
//���ڷ����ַ���
void uart0_send_string(char* str)
{
    //��ǰ�ַ�����ַ���ڽ�β ���� �ַ����׵�ַ��Ϊ��
    while(*str!=0&&str!=0)
    {
        //�����ַ����׵�ַ�е��ַ��������ڷ������֮���׵�ַ����
        uart0_send_char(*str++);
    }
}
void usart0_send_byte(unsigned char byte)
{
	DL_UART_Main_transmitDataBlocking(UART_0_INST, byte);
}
void usart0_send_bytes(unsigned char *buf, int len)
{
  while(len--)
  {
		DL_UART_Main_transmitDataBlocking(UART_0_INST, *buf);
    buf++;
  }
}
uint8_t HAL_UART_Transmit( uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint8_t  *pdata8bits;
	uint8_t TxXferCount;
    pdata8bits  = pData;
	TxXferCount = Size;
    while(TxXferCount > 0U)
    {
		if(pdata8bits != NULL)
		{
			DL_UART_Main_transmitDataBlocking(UART_0_INST, *pdata8bits);
			pdata8bits++;
		}
		TxXferCount--;
		// �ȴ�����Ĵ���Ϊ��   
		while((UART_STAT_TXFE_MASK & (1 << UART_STAT_TXFF_OFS)) != 0);
    }
    return 1;
}

int fputc(int ch, FILE *f)
{
  DL_UART_Main_transmitDataBlocking(UART_0_INST, ch);
  return ch;
}
/*-------------------------------------------------------------------------------------------*/
/*----------------------Ҳ����ʹ��VOFA+�۲첨�Σ�ѡ��JustFloatЭ��---------------------------*/
/*-------------------------------------------------------------------------------------------*/
/*
Ҫ����ʾ:
1. float��unsigned long������ͬ�����ݽṹ����
2. union������������ݴ������ͬ������ռ�
*/
typedef union{
    float fdata;
    unsigned long ldata;
} FloatLongType;
void JustFloat_SendArray(uint8_t *string,uint8_t length)
{
	while(length--)
	{
	DL_UART_Main_transmitDataBlocking(UART_0_INST, *string++);
	}
}
/*��������fת��Ϊ4���ֽ����ݴ����byte[4]��*/
void Float_to_Byte(float f,unsigned char byte[]){
    FloatLongType fl;
    fl.fdata=f;
    byte[0]=(unsigned char)fl.ldata;
    byte[1]=(unsigned char)(fl.ldata>>8);
    byte[2]=(unsigned char)(fl.ldata>>16);
    byte[3]=(unsigned char)(fl.ldata>>24);
}
void JustFloat_Test(void)	//justfloat ����Э�����
{
    float a=1,b=2;	//���͵����� ����ͨ��
	
	u8 byte[4]={0};		//floatת��Ϊ4���ֽ�����
	u8 tail[4]={0x00, 0x00, 0x80, 0x7f};	//֡β
	
	//����λ����������ͨ������
	Float_to_Byte(a,byte);
	//u1_printf("%f\r\n",a);
	JustFloat_SendArray(byte,4);	//1ת��Ϊ4�ֽ����� ����  0x00 0x00 0x80 0x3F
	
	Float_to_Byte(b,byte);
	JustFloat_SendArray(byte,4);	//2ת��Ϊ4�ֽ����� ����  0x00 0x00 0x00 0x40 
	
	//����֡β
	JustFloat_SendArray(tail,4);	//֡βΪ 0x00 0x00 0x80 0x7f

}
//��vofa��������  ��������  ����ͨ��  ���ӻ���ʾ  ֡β
void vofa_sendData(float a,float b,float c){
    u8 byte[4]= {0};//floatת��Ϊ4���ֽ�����
    u8 tail[4]= {0x00, 0x00, 0x80, 0x7f};//֡β

    //����λ������ͨ������
    Float_to_Byte(a,byte);
    JustFloat_SendArray(byte,4);	

    Float_to_Byte(b,byte);
    JustFloat_SendArray(byte,4);	

    Float_to_Byte(c,byte);
    JustFloat_SendArray(byte,4);
    //����֡β
    JustFloat_SendArray(tail,4);	//֡βΪ 0x00 0x00 0x80 0x7f
}

void UART_0_INST_IRQHandler(void)
{
  if(DL_UART_getEnabledInterruptStatus(UART_0_INST,DL_UART_INTERRUPT_RX) == DL_UART_INTERRUPT_RX)
  {
		uint8_t uart_data;
		uart_data =DL_UART_receiveData(UART_0_INST);	//��ȡ���յ�������
		protocol_data_recv(&uart_data, 1);//�����ݷ��뵽���λ�����
		DL_UART_clearInterruptStatus(UART_0_INST,DL_UART_INTERRUPT_RX);//����жϱ�־λ
  }
}
