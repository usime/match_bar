/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���key.c
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/

/************************* ͷ�ļ� *************************/

#include "Button1.h"
#include "gd32f4xx.h"                   // Device header
#include "Button.h"
/************************ ȫ�ֱ������� ************************/

/************************************************************ 
 * Function :       KEY_Init
 * Comment  :       ���ڳ�ʼ��LED�˿�
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/
struct Button btn1;
struct Button btn2;
struct Button btn3;
struct Button btn4;
struct Button btn5;
struct Button btn6;
void KEY1_Init(void)
{
    rcu_periph_clock_enable(KEY_CLK1);								// ��ʼ��KEY����ʱ��
    gpio_mode_set(KEY_CLK1, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                 KEY1_PIN );		//����GPIOģʽΪ��������
		
}
void KEY2_Init(void)
{
    rcu_periph_clock_enable(KEY_CLK2);								// ��ʼ��KEY����ʱ��
   gpio_mode_set(KEY_CLK2, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                 KEY2_PIN|KEY3_PIN|KEY4_PIN|KEY5_PIN|KEY6_PIN );		//����GPIOģʽΪ��������
}

/************************************************************ 
 * Function :       KEY_Stat
 * Comment  :       ���ڶ�ȡ����״̬
 * Parameter:       �����˿ں�����
 * Return   :       ����״̬��1Ϊ�������£�0Ϊ����δ����
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/
uint8_t KEY_Stat(uint32_t port, uint16_t pin)
{
    if(gpio_input_bit_get(port, pin) == RESET)					//��ȡGPIO״̬�����������������
    {
        delay_1ms(20);											//��ʱ����
        if(gpio_input_bit_get(port, pin) == RESET)				//�ٶ�һ��GPIO��������ı���������
        {
            //while(gpio_input_bit_get(port, pin) == RESET);		//�ȴ������ͷ�
            return 1;											//���ذ���״̬1
        }
    }
    return 0;
}
/****************************End*****************************/
//�����
uint8_t read_button_GPIO(uint8_t button_id)
{
	switch(button_id)
	{
		case btn1_id:
			return gpio_input_bit_get(GPIOB,KEY1_PIN);
		case btn2_id:
			return gpio_input_bit_get(GPIOE,KEY2_PIN);
		case btn3_id:
			return gpio_input_bit_get(GPIOE,KEY3_PIN);
		case btn4_id:
			return gpio_input_bit_get(GPIOE,KEY4_PIN);
		case btn5_id:
			return gpio_input_bit_get(GPIOE,KEY5_PIN);
		case btn6_id:
			return gpio_input_bit_get(GPIOE,KEY6_PIN);
		default:
			return 0;
	}
}
