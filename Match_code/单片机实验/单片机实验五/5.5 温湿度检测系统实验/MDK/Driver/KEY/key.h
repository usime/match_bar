#ifndef __KEY_H
#define __KEY_H 			   
#include "stm32f10x.h"
#include "SYS.h"


/* 定义 KEY 引脚 */
#define 	RCC_AHBxPeriph_KEY1    			RCC_APB2Periph_GPIOA
#define 	KEY1_GPIO_PORT					GPIOA
#define 	KEY1_GPIO_PIN					GPIO_Pin_15
#define 	KEY1 							PAin(15)

#define 	RCC_AHBxPeriph_KEY2    			RCC_APB2Periph_GPIOB
#define 	KEY2_GPIO_PORT					GPIOB
#define 	KEY2_GPIO_PIN					GPIO_Pin_4
#define 	KEY2 							PBin(4)

#define 	RCC_AHBxPeriph_KEY3    			RCC_APB2Periph_GPIOA
#define 	KEY3_GPIO_PORT					GPIOB
#define 	KEY3_GPIO_PIN					GPIO_Pin_5
#define 	KEY3 							PBin(5)

void KEY_init(void);

#endif





























