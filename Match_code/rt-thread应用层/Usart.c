#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* 串口设备句柄 */
static rt_device_t serial;
/* 在串口初始化前添加 */
/* 发送字符串到串口 */
void uart_send_string(const char *str)
{
    if (serial != RT_NULL && str != RT_NULL)
    {
        rt_size_t len = rt_strlen(str);
        rt_device_write(serial, 0, str, len);
    }
}

/* 发送指定长度的数据到串口 */
void uart_send_data(const void *data, rt_size_t size)
{
    if (serial != RT_NULL && data != RT_NULL && size > 0)
    {
        rt_device_write(serial, 0, data, size);
    }
}



/* 初始化函数 */
int uart_sample_init(void)
{
    /* 查找串口设备 */
    serial = rt_device_find("uart1");
    if (serial == RT_NULL)
    {
        rt_kprintf("找不到串口设备!\n");
        return RT_ERROR;
    }
    
    /* 以中断接收及轮询发送模式打开串口 */
    if (rt_device_open(serial, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("打开串口设备失败!\n");
        return RT_ERROR;
    }
    
    /* 创建测试线程 */
 
    
    return RT_EOK;
}

/* 导出到msh命令列表 */
MSH_CMD_EXPORT(uart_sample_init, Start UART sample);
