#include "HeaderFiles.h"
#include "ADC.h"
// ����ADC�ɼ����ݵ�����
uint16_t adc_value[2];
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

//DMA1 CH0�ɼ�ADC0������
void DMA_ADC_config_Init(void)
{
		ADC_Port_Init();
		/* ADC regular channel config */
		adc_dma_request_after_last_enable(ADC0);
		adc_dma_mode_enable(ADC0);
		dma_single_data_parameter_struct  dma_single_data_parameter;
		rcu_periph_clock_enable(RCU_DMA1);
    /* DMA������� DMA1 CH0*/
    dma_deinit(DMA1, DMA_CH0);
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(&adc_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;                         // �洢����ַ�Ƿ�Ϊ����ģʽ
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.circular_mode = DMA_CIRCULAR_MODE_ENABLE;                        // ѭ��ģʽ
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;                                // ���赽�洢��
    dma_single_data_parameter.number = 2;                                                      // DMA�����С
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1,DMA_CH0,&dma_single_data_parameter);
    dma_channel_enable(DMA1, DMA_CH0);
		dma_circulation_enable(DMA1, DMA_CH0);        

		//ADC�������
}








