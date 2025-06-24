#ifndef __filter_H
#define __filter_H
#include "stm32f10x.h"
#include "sys.h"

float EWMA_Filter(float current_value);
float ADC_to_LightIntensity(float filtered_adc_value);
#endif 
