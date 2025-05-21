#include <rtthread.h>
#include <rtdevice.h>       // 确保先包含 RT-Thread 设备头文件
#include <board.h>
#include <drv_adc.h>        // 驱动头文件（已包含 rtdevice.h）
#include "ADC.h"            // 自定义头文件

/* 定义ADC通道数量和滑动平均窗口大小 */
#define ADC_CHANNEL_NUM     3       
#define FILTER_WINDOW_SIZE  10      
#define ADC_DEV_NAME        "adc1"          // 与驱动注册的设备名一致

/* ADC采样值缓冲区，二维数组[通道][采样值] */
static uint16_t adc_filter_buf[ADC_CHANNEL_NUM][FILTER_WINDOW_SIZE];
/* 缓冲区索引，记录当前存储位置 */
static uint8_t filter_index[ADC_CHANNEL_NUM] = {0};
/* 滤波后的值 */
static uint16_t filtered_values[ADC_CHANNEL_NUM] = {0};

/* ADC设备句柄（类型已通过 rtdevice.h 定义） */
rt_adc_device_t adc_dev;  // 分号已修正

/* 初始化ADC（与头文件声明一致，返回值为void） */
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

/* 滑动平均滤波函数（保持不变） */
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
    // 使用标准ADC读取接口（注意参数为 &value）
    if (rt_adc_read(adc_dev, channel, &adc_raw_value) != RT_EOK) 
    {
        rt_kprintf("ADC read failed on channel %d\n", channel);
        return 0.0f;
    }
    uint16_t filtered_value = moving_average_filter(channel, adc_raw_value);
    return (float)filtered_value * 3.3f / 4096.0f;
}

/* 导出为组件命令（保持不变） */
MSH_CMD_EXPORT(adc_init, Initialize ADC device);