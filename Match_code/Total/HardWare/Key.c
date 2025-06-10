#include "Key.h"
#include "HeaderFiles.h"

void Key_Init(void){
    rcu_periph_clock_enable(KEY_CLK);								// ��ʼ��KEY����ʱ��
    gpio_mode_set(KEY_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                 KEY3_PIN | KEY4_PIN);		//����GPIOģʽΪ����
}

uint8_t KEY_State(uint32_t port, uint16_t pin)
{
    if(gpio_input_bit_get(port, pin) == RESET)					//��ȡGPIO״̬�����������������
    {
        delay_1ms(20);											//��ʱ����
        if(gpio_input_bit_get(port, pin) == RESET)				//�ٶ�һ��GPIO��������ı���������
        {
            while(gpio_input_bit_get(port, pin) == RESET);		//�ȴ������ͷ�
            return 1;											//���ذ���״̬1
        }
    }
    return 0;
}


