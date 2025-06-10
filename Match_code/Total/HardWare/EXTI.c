#include "HeaderFiles.h"
#include "EXTI.h"
void Test(void){
Led_Flash();
}
void EXTI_Pin_Init(void){
	rcu_periph_clock_enable(RCU_SYSCFG);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOE,EXTI_SOURCE_PIN4);//�����ж���
	exti_init(EXTI_4,EXTI_INTERRUPT,EXTI_TRIG_FALLING);//��ʼ���ⲿ�жϣ�ѡ��ģʽ�ʹ�����ʽ
	nvic_irq_enable(EXTI4_IRQn,2,2);
	exti_interrupt_flag_clear(EXTI_4);
}                           

//�жϷ�����
void EXTI4_IRQHandler(void){
	if(RESET!=exti_interrupt_flag_get(EXTI_4)){
	gpio_bit_toggle(GPIOE,GPIO_PIN_3);//��ƽ��ת
	exti_interrupt_flag_clear(EXTI_4);//����жϱ�־λ
	}
}


