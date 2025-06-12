
#ifndef __RTC_H
#define __RTC_H
#include "HeaderFiles.h"

// ����״̬ö��
typedef enum {
    ADJ_YEAR,
    ADJ_MONTH,
    ADJ_DAY,
    ADJ_HOUR,
    ADJ_MINUTE,
    ADJ_SECOND,
    ADJ_CONFIRM
} AdjustState;
extern AdjustState current_state;



void RTC_Init(void);	// RTC��ʼ��
void rtc_setup(void);	// RTCʱ������
void rtc_show_time(void);	// RTCʱ��
void rtc_show_alarm(void);	// RTC����
uint8_t usart_input_threshold(uint32_t value);  // ��������ֵ��ЧУ��
void rtc_pre_config(void);
// ������ֵ��Χ[min,max]
int32_t CLAMP(int32_t value, int32_t min, int32_t max) ;

// ѭ����ֵ��Χ[min,max]
int32_t CYCLIC(int32_t value, int32_t min, int32_t max) ;

// ��ȡ�����������
uint8_t get_max_day(uint16_t year, uint8_t month) ;
// Ӧ��RTC�޸�
void apply_rtc_changes(void) ;
// ������ֵ
void adjust_value(int8_t delta) ;
// �������¼�
void handle_key_event(uint8_t key) ;
// ��ʾ��ǰ����״̬
void show_adjust_status(void);

void KEY_Adjust(void);
#endif
