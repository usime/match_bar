#ifndef __KEY_H
#define __KEY_H
#include "stdint.h"
void Key_1_2_Init(void);
void Key_3_Init(void);
uint8_t read_button_GPIO(uint8_t button_id);
void BTN1_SingleClick_Handler(void* btn);
void BTN2_SingleClick_Handler(void* btn);
void BTN3_SingleClick_Handler(void* btn);
uint8_t Key_GetNum(void);
enum Button_IDs {
	btn1_id,
	btn2_id,
	btn3_id,
};
#endif
