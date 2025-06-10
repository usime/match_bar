/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���led.h
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/
#ifndef __LED_H
#define __LED_H

/************************* ͷ�ļ� *************************/

#include "HeaderFiles.h"

/************************* �궨�� *************************/

//�����һ��LED״̬����

#define    LED1_OFF()      gpio_bit_reset(GPIOA, GPIO_PIN_4) 	 // �͵�ƽ   �ص� 
#define    LED1_ON()       gpio_bit_set(GPIOA, GPIO_PIN_4)       // �ߵ�ƽ   ����

//����ڶ���LED״̬����
#define    LED2_OFF()      gpio_bit_reset(GPIOA, GPIO_PIN_5) 	 // �͵�ƽ   �ص� 
#define    LED2_ON()       gpio_bit_set(GPIOA, GPIO_PIN_5)       // �ߵ�ƽ   ����

//���������LED״̬����
#define    LED3_OFF()      gpio_bit_reset(GPIOA, GPIO_PIN_6) 	 // �͵�ƽ   �ص� 
#define    LED3_ON()       gpio_bit_set(GPIOA, GPIO_PIN_6)       // �ߵ�ƽ   ���� 

//������Ŀ�LED״̬����
#define    LED4_OFF()      gpio_bit_reset(GPIOA, GPIO_PIN_7) 	 // �͵�ƽ   �ص� 
#define    LED4_ON()       gpio_bit_set(GPIOA, GPIO_PIN_7)       // �ߵ�ƽ   ����
 
/************************ �������� ************************/


/************************ �������� ************************/

void LED_Init(void);     // LED ��ʼ��
				    
#endif


/****************************End*****************************/

