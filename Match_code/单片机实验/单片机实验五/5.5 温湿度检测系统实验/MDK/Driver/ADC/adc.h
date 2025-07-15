#ifndef __ADC_H
#define __ADC_H	
#include <stm32f10x.h>
#include "delay.h"

void ADC1_init(void);
uint16_t Read_Adc1(uint8_t ch);

#endif 
