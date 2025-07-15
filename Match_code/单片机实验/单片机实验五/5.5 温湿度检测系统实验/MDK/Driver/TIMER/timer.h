#ifndef __TIMER_H
#define __TIMER_H
#include <stm32f10x.h>
#include "SYS.h"

void Tim3_init(u16 arr,u16 psc);
void TIM3_IRQHandler(void); 

#endif
