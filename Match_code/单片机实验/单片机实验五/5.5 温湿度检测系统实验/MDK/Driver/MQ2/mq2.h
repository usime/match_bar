#ifndef __MQ2_H
#define __MQ2_H	
#include "sys.h"

/* ¶¨Òå MQ2 Òý½Å */
#define RCC_AHBxPeriph_MQ2    	RCC_APB2Periph_GPIOA
#define MQ2_GPIO_PORT			GPIOA
#define MQ2_GPIO_PIN			GPIO_Pin_6

#define MQ2_Channel				ADC_Channel_6

void MQ2_init(void);
uint16_t Read_MQ2(uint8_t ch);
#endif 
