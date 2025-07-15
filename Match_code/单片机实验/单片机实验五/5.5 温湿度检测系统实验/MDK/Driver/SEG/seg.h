#ifndef __SEG_H
#define __SEG_H
#include "stm32f10x.h"
#include "sys.h"


/* ¶¨Òå SEG Òý½Å */
#define 	RCC_AHBxPeriph_SEG_CS    		RCC_APB2Periph_GPIOA
#define  	SEG_CS_GPIO_PORT				GPIOA
#define  	SEG_CS_GPIO_PIN   				GPIO_Pin_4
#define		SEG_CS							PAout(4)

#define 	RCC_AHBxPeriph_SEG_SCK   		RCC_APB2Periph_GPIOA
#define 	SEG_SCK_GPIO_PORT      			GPIOA
#define  	SEG_SCK_GPIO_PIN   				GPIO_Pin_5
#define 	SEG_SCK							PAout(5)


#define 	RCC_AHBxPeriph_SEG_MOSI    		RCC_APB2Periph_GPIOA
#define  	SEG_MOSI_GPIO_PORT				GPIOA
#define  	SEG_MOSI_GPIO_PIN   			GPIO_Pin_7
#define		SEG_MOSI						PAout(7)


void SEG_init(void);
void Write_Seg(u8 num,u8 com);

#endif 
