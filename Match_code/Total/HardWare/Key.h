#ifndef __KEY_H
#include "HeaderFiles.h"

// KEY���Ŷ���
#define KEY1_PIN        GPIO_PIN_2
#define KEY2_PIN        GPIO_PIN_3
#define KEY3_PIN        GPIO_PIN_4
#define KEY4_PIN        GPIO_PIN_5

// KEY�˿ڶ���
#define KEY_PORT        GPIOE
#define KEY_CLK         RCU_GPIOE

void Key_Init(void);									//������ʼ��
uint8_t KEY_State(uint32_t port, uint16_t pin);		//����״̬ɨ��

#define __KEY_H
#endif

