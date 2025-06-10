#include "HeaderFiles.h"
#include "ADC.h"
void ADC_Port_Init(void){
	rcu_periph_clock_enable(RCU_GPIOC);//����Ҫ������ʱ��
	rcu_periph_clock_enable(RCU_ADC0);//ADC0ʱ�Ӵ�
	gpio_mode_set(GPIOC,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_0);//����ģʽΪģ����
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);//ѡ��ʱ�ӷ�Ƶ
	ADC_Init();
	adc_software_trigger_enable(ADC0,ADC_ROUTINE_CHANNEL);
}

void ADC_Init(void){
	adc_deinit();
	adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,ENABLE);//ѡ��ADC��ģʽ����ɨ�裬������
	adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);//ѡ�����ݶ��뷽ʽ
	adc_channel_length_config(ADC0,ADC_ROUTINE_CHANNEL,1);//����ͨ��
	
	adc_routine_channel_config(ADC0,0,ADC_CHANNEL_10,ADC_SAMPLETIME_480);//���ô���Դ�Ͳ���ʱ��
	adc_external_trigger_source_config(ADC0,ADC_ROUTINE_CHANNEL,ADC_EXTTRIG_INSERTED_T0_CH3);//���ô���Դ
	
	adc_external_trigger_config(ADC0,ADC_ROUTINE_CHANNEL,EXTERNAL_TRIGGER_RISING_FALLING);//����ʹ��
	adc_enable(ADC0);//ADCʹ��
	delay_1ms(1);
	
	adc_calibration_enable(ADC0);//У׼
	
}


