#include <rtthread.h>
#include <rtdevice.h>       // ȷ���Ȱ��� RT-Thread �豸ͷ�ļ�
#include <board.h>
#include <drv_adc.h>        // ����ͷ�ļ����Ѱ��� rtdevice.h��
#include "ADC.h"            // �Զ���ͷ�ļ�

/* ����ADCͨ�������ͻ���ƽ�����ڴ�С */
#define ADC_CHANNEL_NUM     3       
#define FILTER_WINDOW_SIZE  10      
#define ADC_DEV_NAME        "adc1"          // ������ע����豸��һ��

/* ADC����ֵ����������ά����[ͨ��][����ֵ] */
static uint16_t adc_filter_buf[ADC_CHANNEL_NUM][FILTER_WINDOW_SIZE];
/* ��������������¼��ǰ�洢λ�� */
static uint8_t filter_index[ADC_CHANNEL_NUM] = {0};
/* �˲����ֵ */
static uint16_t filtered_values[ADC_CHANNEL_NUM] = {0};

/* ADC�豸�����������ͨ�� rtdevice.h ���壩 */
rt_adc_device_t adc_dev;  // �ֺ�������

/* ��ʼ��ADC����ͷ�ļ�����һ�£�����ֵΪvoid�� */
void adc_init(void)
{
    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("ADC device not found!\n");
        return;
    }
    rt_kprintf("ADC initialized successfully\n");
}

/* ����ƽ���˲����������ֲ��䣩 */
static uint16_t moving_average_filter(uint8_t channel, uint16_t raw_value)
{
    uint32_t sum = 0;
    int i;
    adc_filter_buf[channel][filter_index[channel]] = raw_value;
    filter_index[channel] = (filter_index[channel] + 1) % FILTER_WINDOW_SIZE;
    for (i = 0; i < FILTER_WINDOW_SIZE; i++) sum += adc_filter_buf[channel][i];
    return (uint16_t)(sum / FILTER_WINDOW_SIZE);
}

float adc_read(uint8_t channel)
{
    uint16_t adc_raw_value;
    if (channel >= ADC_CHANNEL_NUM) 
    {
        rt_kprintf("Error: Invalid ADC channel %d!\n", channel);
        return 0.0f;
    }
    // ʹ�ñ�׼ADC��ȡ�ӿڣ�ע�����Ϊ &value��
    if (rt_adc_read(adc_dev, channel, &adc_raw_value) != RT_EOK) 
    {
        rt_kprintf("ADC read failed on channel %d\n", channel);
        return 0.0f;
    }
    uint16_t filtered_value = moving_average_filter(channel, adc_raw_value);
    return (float)filtered_value * 3.3f / 4096.0f;
}

/* ����Ϊ���������ֲ��䣩 */
MSH_CMD_EXPORT(adc_init, Initialize ADC device);