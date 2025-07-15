#ifndef __BEEP_H
#define __BEEP_H 			   
#include "SYS.h"

/* 定义 BEEP 引脚 */
#define 	RCC_AHBxPeriph_BEEP    	RCC_APB2Periph_GPIOA
#define 	BEEP_GPIO_PORT			GPIOA
#define 	BEEP_GPIO_PIN			GPIO_Pin_8

#define 	BEEP 					PAout(8)

void BEEP_init(void);
void BEEP_ON();
void BEEP_OFF();
#endif





























