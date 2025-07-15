
#include "atk_esp8266d_uart.h"
#include "usart2.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static struct
{
    uint8_t buf[ATK_ESP8266D_UART_RX_BUF_SIZE];              /* 帧接收缓冲 */
    struct
    {
        uint16_t len    : 15;                               /* 帧接收长度，sta[14:0] */
        uint16_t finsh  : 1;                                /* 帧接收完成标志，sta[15] */
    } sta;                                                  /* 帧状态信息 */
} g_uart_rx_frame = {0};                                    /* ATK-ESP8266D UART接收帧缓冲信息结构体 */

static uint8_t g_uart_tx_buf[ATK_ESP8266D_UART_TX_BUF_SIZE]; /* ATK-ESP8266D UART发送缓冲 */


/* 串口发送数据 参数1：串口号，参数2：数据指针，参数3：数据长度。*/
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
 * @param       fmt: 待打印的数据
 * @retval      无
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
 * @brief       ATK-ESP8266D UART重新开始接收数据
 * @param       无
 * @retval      无
 */
void atk_esp8266d_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len     = 0;
    g_uart_rx_frame.sta.finsh   = 0;
}

/**
 * @brief       获取ATK-ESP8266D UART接收到的一帧数据
 * @param       无
 * @retval      NULL: 未接收到一帧数据
 *              其他: 接收到的一帧数据
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
 * @brief       获取ATK-ESP8266D UART接收到的一帧数据的长度
 * @param       无
 * @retval      0   : 未接收到一帧数据
 *              其他: 接收到的一帧数据的长度
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
 * @brief       ATK-ESP8266D USART初始化
 * @param       baudrate: USART通讯波特率
 * @retval      无
 */
void atk_esp8266d_uart_init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ATK_ESP8266D_UART|RCC_AHBxPeriph_ATK_ESP8266D_UART_TX|RCC_AHBxPeriph_ATK_ESP8266D_UART_RX, ENABLE);	//使能串口和GPIO时钟时钟

	GPIO_InitStructure.GPIO_Pin = ATK_ESP8266D_UART_TX_GPIO_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(ATK_ESP8266D_UART_TX_GPIO_PORT, &GPIO_InitStructure);					//初始化发送GPIO

	GPIO_InitStructure.GPIO_Pin = ATK_ESP8266D_UART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ATK_ESP8266D_UART_RX_GPIO_PORT, &GPIO_InitStructure);					//初始化接收GPIO

	//USART初始化设置
	USART_InitStructure.USART_BaudRate = baudrate;									//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(ATK_ESP8266D_UART_INTERFACE, &USART_InitStructure); 					//初始化串口

	USART_ITConfig(ATK_ESP8266D_UART_INTERFACE, USART_IT_RXNE, ENABLE);				//开启串口接受中断
	USART_ITConfig(ATK_ESP8266D_UART_INTERFACE, USART_IT_IDLE, ENABLE);				//开启串口空闲中断
	USART_Cmd(ATK_ESP8266D_UART_INTERFACE, ENABLE);                    				//使能串口

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = ATK_ESP8266D_UART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;						//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//初始化VIC寄存器
}

/**
 * @brief       ATK-ESP8266D UART中断回调函数
 * @param       无
 * @retval      无
 */
void ATK_ESP8266D_UART_IRQHandler(void)
{
    uint8_t tmp;  
    if(USART_GetFlagStatus(ATK_ESP8266D_UART_INTERFACE, USART_FLAG_RXNE) != RESET)        			/* UART接收中断 */
    {           
		tmp = USART_ReceiveData(ATK_ESP8266D_UART_INTERFACE);										/* UART接收数据 */
        if (g_uart_rx_frame.sta.len < (ATK_ESP8266D_UART_RX_BUF_SIZE - 1))   						/* 判断UART接收缓冲是否溢出                                                                            * 留出一位给结束符'\0'*/
        {
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             						/* 将接收到的数据写入缓冲 */
            g_uart_rx_frame.sta.len++;                                      						/* 更新接收到的数据长度 */
        }
        else                                                                						/* UART接收缓冲溢出 */
        {
            g_uart_rx_frame.sta.len = 0;                                   							/* 覆盖之前收到的数据 */
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             						/* 将接收到的数据写入缓冲 */
            g_uart_rx_frame.sta.len++;                                      						/* 更新接收到的数据长度 */
        }
		USART_ClearFlag(ATK_ESP8266D_UART_INTERFACE,USART_FLAG_RXNE);
    }
	if(USART_GetFlagStatus(ATK_ESP8266D_UART_INTERFACE,USART_FLAG_IDLE)  != RESET)					/* UART总线空闲中断 */
    {
        g_uart_rx_frame.sta.finsh = 1;  															/* 标记帧接收完成 */
  		ATK_ESP8266D_UART_INTERFACE->SR;															/* 先读SR寄存器，再读DR寄存器 */			
		ATK_ESP8266D_UART_INTERFACE->DR;
		UART_Transmit(USART2, g_uart_rx_frame.buf, g_uart_rx_frame.sta.len);						/* 使用串口2打印从ESP8266上接到的数据*/		
    }
}
