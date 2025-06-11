#include "HeaderFiles.h"
#include "Timer.h"
int cnt=0;

// ��ʱ�� 2 ��ʼ������
void TIM2_Init(uint16_t prescaler, uint16_t period) {
    // ʹ�ܶ�ʱ�� 2 ʱ��
    rcu_periph_clock_enable(RCU_TIMER2);
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//ѡ��ʱ��Դ��ƵΪ240Mhz
    // ��ʱ����������
    timer_parameter_struct timer_initpara;
    timer_deinit(TIMER2);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler = prescaler; // ����Ԥ��Ƶ�� psc
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE; // ��Ե����ģʽ
    timer_initpara.counterdirection = TIMER_COUNTER_UP; // ���ϼ���
    timer_initpara.period = period; // �����Զ�����ֵarr
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1; // ʱ�ӷ�Ƶ����
    timer_initpara.repetitioncounter = 0; // �ظ�������
    timer_init(TIMER2, &timer_initpara);
    // ʹ���Զ�����Ԥװ��
    timer_auto_reload_shadow_enable(TIMER2);
    // �����ж����ȼ�
    nvic_irq_enable(TIMER2_IRQn, 0, 0);
    // ʹ������ж�
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    // ������ʱ��
    timer_enable(TIMER2);
}

// ��ʱ�� 2 ����жϷ������
void TIMER2_IRQHandler(void) {
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP) != RESET) {
        cnt++;  
		button_ticks();
		// ��������жϱ�־
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
		//cnt++;  
				
    }
}



