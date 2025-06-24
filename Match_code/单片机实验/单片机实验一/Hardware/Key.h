#ifndef __KEY_H
#define __KEY_H
#include "stdint.h"
enum Button_IDs {
	btn1_id,
	btn2_id,
	btn3_id,
};
void Key_1_2_Init(void);
void Key_3_Init(void);
uint8_t read_button_GPIO(uint8_t button_id);


#endif
