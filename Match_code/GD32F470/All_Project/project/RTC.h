/************************************************************
 * ��?������o2025CIMC Copyright?�� 
 * ???t��oRTC.h
 * ����??: Qiao Qin @ GigaDevice
 * ??����: 2025CIMC IHD-V04
 * ��?��?: Qiao Qin     2025/4/20     V0.01    original
************************************************************/

#ifndef __RTC_H
#define __RTC_H


void RTC_Init(void);	// RTC3?��??��
void rtc_setup(void);	// RTC����?������??
void rtc_show_time(void);	// RTC����??
void rtc_show_alarm(void);	// RTC???��
uint8_t usart_input_threshold(uint32_t value);  // ��?���¨�?��??�̨�DD��D��?��
void rtc_pre_config(void);

#endif
