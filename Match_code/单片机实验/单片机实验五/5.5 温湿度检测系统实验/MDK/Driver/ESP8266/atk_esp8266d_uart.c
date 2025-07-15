
#include "atk_esp8266d_uart.h"
#include "usart2.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static struct
{
    uint8_t buf[ATK_ESP8266D_UART_RX_BUF_SIZE];              /* ֡���ջ��� */
    struct
    {
        uint16_t len    : 15;                               /* ֡���ճ��ȣ�sta[14:0] */
        uint16_t finsh  : 1;                                /* ֡������ɱ�־��sta[15] */
    } sta;                                                  /* ֡״̬��Ϣ */
} g_uart_rx_frame = {0};                                    /* ATK-ESP8266D UART����֡������Ϣ�ṹ�� */

static uint8_t g_uart_tx_buf[ATK_ESP8266D_UART_TX_BUF_SIZE]; /* ATK-ESP8266D UART���ͻ��� */


/* ���ڷ������� ����1�����ںţ�����2������ָ�룬����3�����ݳ��ȡ�*/
void UART_Transmit(USART_TypeDef* USARTx, uint8_t *pData, uint16_t Size)
{
	while(Size > 0)
	{
		while( (USARTx->SR & 0X40) == 0 );  
		USARTx->DR = *pData;  
		pData++;		
		Size --;
	}
}

/**
 * @brief       ATK-ESP8266D UART printf
 * @param       fmt: ����ӡ������
 * @retval      ��
 */
void atk_esp8266d_uart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;
    
    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);
    
    len = strlen((const char *)g_uart_tx_buf);
    UART_Transmit(ATK_ESP8266D_UART_INTERFACE, g_uart_tx_buf, len);
}

/**
 * @brief       ATK-ESP8266D UART���¿�ʼ��������
 * @param       ��
 * @retval      ��
 */
void atk_esp8266d_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len     = 0;
    g_uart_rx_frame.sta.finsh   = 0;
}

/**
 * @brief       ��ȡATK-ESP8266D UART���յ���һ֡����
 * @param       ��
 * @retval      NULL: δ���յ�һ֡����
 *              ����: ���յ���һ֡����
 */
uint8_t *atk_esp8266d_uart_rx_get_frame(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = '\0';
        return g_uart_rx_frame.buf;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief       ��ȡATK-ESP8266D UART���յ���һ֡���ݵĳ���
 * @param       ��
 * @retval      0   : δ���յ�һ֡����
 *              ����: ���յ���һ֡���ݵĳ���
 */
uint16_t atk_esp8266d_uart_rx_get_frame_len(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        return g_uart_rx_frame.sta.len;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       ATK-ESP8266D USART��ʼ��
 * @param       baudrate: USARTͨѶ������
 * @retval      ��
 */
void atk_esp8266d_uart_init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ATK_ESP8266D_UART|RCC_AHBxPeriph_ATK_ESP8266D_UART_TX|RCC_AHBxPeriph_ATK_ESP8266D_UART_RX, ENABLE);	//ʹ�ܴ��ں�GPIOʱ��ʱ��

	GPIO_InitStructure.GPIO_Pin = ATK_ESP8266D_UART_TX_GPIO_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(ATK_ESP8266D_UART_TX_GPIO_PORT, &GPIO_InitStructure);					//��ʼ������GPIO

	GPIO_InitStructure.GPIO_Pin = ATK_ESP8266D_UART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ATK_ESP8266D_UART_RX_GPIO_PORT, &GPIO_InitStructure);					//��ʼ������GPIO

	//USART��ʼ������
	USART_InitStructure.USART_BaudRate = baudrate;									//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	USART_Init(ATK_ESP8266D_UART_INTERFACE, &USART_InitStructure); 					//��ʼ������

	USART_ITConfig(ATK_ESP8266D_UART_INTERFACE, USART_IT_RXNE, ENABLE);				//�������ڽ����ж�
	USART_ITConfig(ATK_ESP8266D_UART_INTERFACE, USART_IT_IDLE, ENABLE);				//�������ڿ����ж�
	USART_Cmd(ATK_ESP8266D_UART_INTERFACE, ENABLE);                    				//ʹ�ܴ���

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = ATK_ESP8266D_UART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;						//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);													//��ʼ��VIC�Ĵ���
}

/**
 * @brief       ATK-ESP8266D UART�жϻص�����
 * @param       ��
 * @retval      ��
 */
void ATK_ESP8266D_UART_IRQHandler(void)
{
    uint8_t tmp;  
    if(USART_GetFlagStatus(ATK_ESP8266D_UART_INTERFACE, USART_FLAG_RXNE) != RESET)        			/* UART�����ж� */
    {           
		tmp = USART_ReceiveData(ATK_ESP8266D_UART_INTERFACE);										/* UART�������� */
        if (g_uart_rx_frame.sta.len < (ATK_ESP8266D_UART_RX_BUF_SIZE - 1))   						/* �ж�UART���ջ����Ƿ����                                                                            * ����һλ��������'\0'*/
        {
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             						/* �����յ�������д�뻺�� */
            g_uart_rx_frame.sta.len++;                                      						/* ���½��յ������ݳ��� */
        }
        else                                                                						/* UART���ջ������ */
        {
            g_uart_rx_frame.sta.len = 0;                                   							/* ����֮ǰ�յ������� */
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             						/* �����յ�������д�뻺�� */
            g_uart_rx_frame.sta.len++;                                      						/* ���½��յ������ݳ��� */
        }
		USART_ClearFlag(ATK_ESP8266D_UART_INTERFACE,USART_FLAG_RXNE);
    }
	if(USART_GetFlagStatus(ATK_ESP8266D_UART_INTERFACE,USART_FLAG_IDLE)  != RESET)					/* UART���߿����ж� */
    {
        g_uart_rx_frame.sta.finsh = 1;  															/* ���֡������� */
  		ATK_ESP8266D_UART_INTERFACE->SR;															/* �ȶ�SR�Ĵ������ٶ�DR�Ĵ��� */			
		ATK_ESP8266D_UART_INTERFACE->DR;
		UART_Transmit(USART2, g_uart_rx_frame.buf, g_uart_rx_frame.sta.len);						/* ʹ�ô���2��ӡ��ESP8266�Ͻӵ�������*/		
    }
}
