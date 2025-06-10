/************************************************************
 * ～?豕“㏒o2025CIMC Copyright?㏒ 
 * ???t㏒oRTC.h
 * ℅¯??: Qiao Qin @ GigaDevice
 * ??足“: 2025CIMC IHD-V04
 * ～?㊣?: Qiao Qin     2025/4/20     V0.01    original
************************************************************/

#ifndef __RTC_H
#define __RTC_H


void RTC_Init(void);	// RTC3?那??‘
void rtc_setup(void);	// RTC那㊣?車谷豕??
void rtc_show_time(void);	// RTC那㊣??
void rtc_show_alarm(void);	// RTC???車
uint8_t usart_input_threshold(uint32_t value);  // 車?℅¯那?豕??米車DD∫D㏒?谷
void rtc_pre_config(void);

#endif
