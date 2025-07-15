
#ifndef __ATK_ESP8266D_UART_H
#define __ATK_ESP8266D_UART_H

#include "sys.h"

/* 引脚定义 */
#define ATK_ESP8266D_UART_TX_GPIO_PORT           GPIOA
#define ATK_ESP8266D_UART_TX_GPIO_PIN            GPIO_Pin_9
#define RCC_AHBxPeriph_ATK_ESP8266D_UART_TX		 RCC_APB2Periph_GPIOA

#define ATK_ESP8266D_UART_RX_GPIO_PORT           GPIOA
#define ATK_ESP8266D_UART_RX_GPIO_PIN            GPIO_Pin_10
#define RCC_AHBxPeriph_ATK_ESP8266D_UART_RX		 RCC_APB2Periph_GPIOA

#define ATK_ESP8266D_UART_INTERFACE              USART1
#define ATK_ESP8266D_UART_IRQn                   USART1_IRQn
#define ATK_ESP8266D_UART_IRQHandler             USART1_IRQHandler
#define RCC_APB2Periph_ATK_ESP8266D_UART 		 RCC_APB2Periph_USART1

/* UART收发缓冲大小 */
#define ATK_ESP8266D_UART_RX_BUF_SIZE            512
#define ATK_ESP8266D_UART_TX_BUF_SIZE            256

/* 操作函数 */
void UART_Transmit(USART_TypeDef* USARTx, uint8_t *pData, uint16_t Size);
void atk_esp8266d_uart_printf(char *fmt, ...);       /* ATK-ESP8266D UART printf */
void atk_esp8266d_uart_rx_restart(void);             /* ATK-ESP8266D UART重新开始接收数据 */
uint8_t *atk_esp8266d_uart_rx_get_frame(void);       /* 获取ATK-ESP8266D UART接收到的一帧数据 */
uint16_t atk_esp8266d_uart_rx_get_frame_len(void);   /* 获取ATK-ESP8266D UART接收到的一帧数据的长度 */
void atk_esp8266d_uart_init(uint32_t baudrate);      /* ATK-ESP8266D UART初始化 */

#endif
