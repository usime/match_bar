/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���led.c
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/

/************************* ͷ�ļ� *************************/

#include "LED.h"

/************************ ȫ�ֱ������� ************************/


/************************************************************ 
 * Function :       LED_Init
 * Comment  :       ���ڳ�ʼ��LED�˿�
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void LED_Init(void)
{
	
	rcu_periph_clock_enable(RCU_GPIOA);    // ��ʼ��GPIO_A����ʱ��
	
	//��ʼ��LED1�˿�
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);   			// GPIOģʽ����Ϊ���
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);   // �����������
	gpio_bit_reset(GPIOA, GPIO_PIN_4);  											// ���ų�ʼ��ƽΪ�͵�ƽ
	
	//��ʼ��LED2~4�˿ڣ�������ʼ���˿ڣ�
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);   			// GPIOģʽ����Ϊ���
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);     // �����������
	gpio_bit_reset(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);  										     	// ���ų�ʼ��ƽΪ�͵�ƽ
	
	
}



/****************************End*****************************/

