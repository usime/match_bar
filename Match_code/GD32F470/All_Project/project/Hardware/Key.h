/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���key.h
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/
#ifndef __KEY_H
#define __KEY_H

/************************* ͷ�ļ� *************************/

#include "HeaderFiles.h"

/************************* �궨�� *************************/

// KEY���Ŷ���
#define KEY1_PIN        GPIO_PIN_0
#define KEY2_PIN        GPIO_PIN_7
#define KEY3_PIN        GPIO_PIN_9
#define KEY4_PIN        GPIO_PIN_11
#define KEY5_PIN        GPIO_PIN_13
#define KEY6_PIN        GPIO_PIN_15
enum Button_IDs {
	btn1_id,
	btn2_id,
	btn3_id,
	btn4_id,
	btn5_id,
	btn6_id,
};
// KEY�˿ڶ���
#define KEY_PORT        GPIOE
#define KEY_CLK1         RCU_GPIOB
#define KEY_CLK2         RCU_GPIOE
/************************ �������� ************************/


/************************ �������� ************************/
uint8_t read_button_GPIO(uint8_t button_id);
void KEY1_Init(void);									//������ʼ��
uint8_t KEY_Stat(uint32_t port, uint16_t pin);		//����״̬ɨ��
void KEY2_Init(void);				    
#endif


/****************************End*****************************/

