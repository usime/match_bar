#include "HeaderFiles.h"
#include "ADC.h"

// 定义ADC采集数据的数组
uint16_t adc_value[2];

void ADC_Port_Init(void) {
    rcu_periph_clock_enable(RCU_GPIOC); // 打开GPIOC时钟
    rcu_periph_clock_enable(RCU_ADC0);  // 打开ADC0时钟
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0); // PC0设置为模拟输入
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8); // 配置ADC时钟分频
}

void ADC_Init(void) {
    adc_deinit(); // ADC去初始化
    // 配置ADC为连续模式
		adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);               // 扫描模式
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
		adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);                         //独立模式
    // 数据对齐方式设为右对齐
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    // 配置ADC通道数为1
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);
		adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);                  // 右对齐
		adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);              //读取2通信
    // 配置ADC通道0，采样时间为480个周期
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_480);
    // ADC使能
    adc_enable(ADC0);
    // 延时等待ADC稳定
    delay_1ms(1);
    // ADC校准
    adc_calibration_enable(ADC0);
		adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}
//DMA1 CH0采集ADC0的数据
void DMA_ADC_config_Init(void)
{
		ADC_Port_Init();
		ADC_Init();
		/* ADC regular channel config */
		adc_dma_request_after_last_enable(ADC0);
		adc_dma_mode_enable(ADC0);
		dma_single_data_parameter_struct  dma_single_data_parameter;
		rcu_periph_clock_enable(RCU_DMA1);
    /* DMA相关配置 DMA1 CH0*/
    dma_deinit(DMA1, DMA_CH0);
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(&adc_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;                         // 存储器地址是否为增量模式
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.circular_mode = DMA_CIRCULAR_MODE_ENABLE;                        // 循环模式
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;                                // 外设到存储器
    dma_single_data_parameter.number = 2;                                                      // DMA缓存大小
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1,DMA_CH0,&dma_single_data_parameter);
    dma_channel_enable(DMA1, DMA_CH0);
		dma_circulation_enable(DMA1, DMA_CH0);        

		//ADC相关配置

}
