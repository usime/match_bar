#ifndef __KEY_H
#define __KEY_H
#include "stdint.h"
uint8_t read_button_GPIO(uint8_t button_id);
void BTN1_SingleClick_Handler(void);
void BTN2_SingleClick_Handler(void);
void BTN3_SingleClick_Handler(void);
enum Button_IDs {
	btn1_id,
	btn2_id,
	btn3_id,
};
//这个ID是用来button_attach函数绑定某个按键ID
#endif
