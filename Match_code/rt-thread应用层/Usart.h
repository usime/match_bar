#ifndef _Usart_H__
#define _Usart_H__
void uart_send_string(const char *str);
void uart_send_data(const void *data, rt_size_t size);

int uart_sample_init(void);

#endif