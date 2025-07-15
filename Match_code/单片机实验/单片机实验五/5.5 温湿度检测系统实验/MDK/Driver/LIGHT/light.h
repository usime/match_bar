#ifndef __LIGHT_H
#define __LIGHT_H	
#include "sys.h"

/* ¶¨Òå LIGHT Òý½Å */
#define RCC_AHBxPeriph_LIGHT    RCC_APB2Periph_GPIOA
#define LIGHT_GPIO_PORT			GPIOA
#define LIGHT_GPIO_PIN			GPIO_Pin_0

#define LIGHT_Channel			ADC_Channel_0

void LIGHT_init(void);
uint16_t Read_LIGHT(uint8_t ch);
#endif 
