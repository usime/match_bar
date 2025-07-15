#ifndef __LED_H
#define __LED_H 			   

#include "SYS.h"

/* ¶¨Òå LED Òý½Å */
#define 	RCC_AHBxPeriph_LED    	RCC_APB2Periph_GPIOB
#define 	LED_GPIO_PORT			GPIOB
#define 	LED_GPIO_PIN			GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |GPIO_Pin_15

#define LED1 PBout(12)
#define LED2 PBout(13)
#define LED3 PBout(14)
#define LED4 PBout(15)


void LED_init(void);

#endif





























