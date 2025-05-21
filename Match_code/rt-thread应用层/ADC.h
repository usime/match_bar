#ifndef __ADC__H
#define __ADC__H
#include <rtthread.h>
uint16_t get_filtered_adc_value(uint8_t channel);
int adc_init(void);
#endif