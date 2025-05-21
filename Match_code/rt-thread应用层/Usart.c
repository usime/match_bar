#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* �����豸��� */
static rt_device_t serial;
/* �ڴ��ڳ�ʼ��ǰ��� */
/* �����ַ��������� */
void uart_send_string(const char *str)
{
    if (serial != RT_NULL && str != RT_NULL)
    {
        rt_size_t len = rt_strlen(str);
        rt_device_write(serial, 0, str, len);
    }
}

/* ����ָ�����ȵ����ݵ����� */
void uart_send_data(const void *data, rt_size_t size)
{
    if (serial != RT_NULL && data != RT_NULL && size > 0)
    {
        rt_device_write(serial, 0, data, size);
    }
}



/* ��ʼ������ */
int uart_sample_init(void)
{
    /* ���Ҵ����豸 */
    serial = rt_device_find("uart1");
    if (serial == RT_NULL)
    {
        rt_kprintf("�Ҳ��������豸!\n");
        return RT_ERROR;
    }
    
    /* ���жϽ��ռ���ѯ����ģʽ�򿪴��� */
    if (rt_device_open(serial, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("�򿪴����豸ʧ��!\n");
        return RT_ERROR;
    }
    
    /* ���������߳� */
 
    
    return RT_EOK;
}

/* ������msh�����б� */
MSH_CMD_EXPORT(uart_sample_init, Start UART sample);
