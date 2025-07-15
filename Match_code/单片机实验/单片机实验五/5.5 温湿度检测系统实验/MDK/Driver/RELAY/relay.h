#ifndef __RELAY_H
#define __RELAY_H 			   
#include "SYS.h"

/* 定义 RELAY 引脚 */
#define 	RCC_AHBxPeriph_RELAY    RCC_APB2Periph_GPIOA
#define 	RELAY_GPIO_PORT			GPIOA
#define 	RELAY_GPIO_PIN			GPIO_Pin_11

#define 	RELAY 					PAout(11)

void RELAY_init(void);

#endif





























