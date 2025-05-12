#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_SetCompare1(uint16_t compare);//假设为左
void PWM_SetCompare2(uint16_t compare);//假设为右

#endif

