//���ļ���ATָ����ͺͷ�������ƥ��
//ATK_ESP8266D_EOK���ɹ�
//ATK_ESP8266D_ERROR��ʧ��

#include "atk_esp8266d.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*************************************************************************************************
 * @brief       ATK-ESP8266D����ATָ��
 * @param       cmd    : �����͵�ATָ��
 *              ack    : �ȴ�����Ӧ
 *              timeout: �ȴ���ʱʱ��
 * @retval      ATK_ESP8266D_EOK     : ����ִ�гɹ�
 *              ATK_ESP8266D_ETIMEOUT: �ȴ�����Ӧ��ʱ������ִ��ʧ��
 */
unsigned char atk_esp8266d_send_at_cmd(char *cmd, char *ack, uint32_t timeout)
{
    unsigned char *ret = NULL;
    
    atk_esp8266d_uart_rx_restart();
    atk_esp8266d_uart_printf("%s\r\n", cmd);
    if ((ack == NULL) || (timeout == 0))
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        while (timeout > 0)
        {
            ret = atk_esp8266d_uart_rx_get_frame();
            if (ret != NULL)
            {
                if (strstr((const char *)ret, ack) != NULL)
                {
                    return ATK_ESP8266D_EOK;
                }
                else
                {
                    atk_esp8266d_uart_rx_restart();
                }
            }
            timeout--;
            atk_esp8266d_delay_ms(1);
        } 
        return ATK_ESP8266D_ETIMEOUT;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D��ʼ��
 * @param       baudrate: ATK-ESP8266D UARTͨѶ������
 * @retval      ATK_ESP8266D_EOK  : ATK-ESP8266D��ʼ���ɹ�������ִ�гɹ�
 *              ATK_ESP8266D_ERROR: ATK-ESP8266D��ʼ��ʧ�ܣ�����ִ��ʧ��
 */
unsigned char atk_esp8266d_init(uint32_t baudrate)
{
    atk_esp8266d_uart_init(baudrate);                /* ATK-ESP8266D UART��ʼ�� */
    if (atk_esp8266d_at_test() != ATK_ESP8266D_EOK)   /* ATK-ESP8266D ATָ����� */
    {
        return ATK_ESP8266D_ERROR;
    }
    
    return ATK_ESP8266D_EOK;
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D ATָ�����
 * @param       ��
 * @retval      ATK_ESP8266D_EOK  : ATָ����Գɹ�
 *              ATK_ESP8266D_ERROR: ATָ�����ʧ��
 */
unsigned char atk_esp8266d_at_test(void)
{
    unsigned char ret;
    unsigned char i;
    for (i=0; i<10; i++)
    {
        ret = atk_esp8266d_send_at_cmd("AT", "OK", 500);
        if (ret == ATK_ESP8266D_EOK)
        {
            return ATK_ESP8266D_EOK;
        }
    }
    return ATK_ESP8266D_ERROR;
}


//����ATָ�
/*************************************************************************************************
 * @brief       ATK-ESP8266D�ָ���������
 * @param       ��
 * @retval      ATK_ESP8266D_EOK  : �ָ��������óɹ�
 *              ATK_ESP8266D_ERROR: �ָ���������ʧ��
 */
unsigned char atk_esp8266d_restore(void)
{
    unsigned char ret;
    
    ret = atk_esp8266d_send_at_cmd("AT+RESTORE", "ready", 3000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D�����λ
 * @param       ��
 * @retval      ATK_ESP8266D_EOK  : �����λ�ɹ�
 *              ATK_ESP8266D_ERROR: �����λʧ��
 */
unsigned char atk_esp8266d_sw_reset(void)
{
    unsigned char ret;
    
    ret = atk_esp8266d_send_at_cmd("AT+RST", "OK", 500);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ����ATK-ESP8266D����ģʽ
 * @param       mode: 1��Stationģʽ
 *                    2��APģʽ
 *                    3��AP+Stationģʽ
 * @retval      ATK_ESP8266D_EOK   : ����ģʽ���óɹ�
 *              ATK_ESP8266D_ERROR : ����ģʽ����ʧ��
 *              ATK_ESP8266D_EINVAL: mode�������󣬹���ģʽ����ʧ��
 */
unsigned char atk_esp8266d_set_mode(unsigned char mode)
{
    unsigned char ret;
    switch (mode)
    {
        case 1:
        {
            ret = atk_esp8266d_send_at_cmd("AT+CWMODE=1", "OK", 500);    /* Stationģʽ */
            break;
        }
        case 2:
        {
            ret = atk_esp8266d_send_at_cmd("AT+CWMODE=2", "OK", 500);    /* APģʽ */
            break;
        }
        case 3:
        {
            ret = atk_esp8266d_send_at_cmd("AT+CWMODE=3", "OK", 500);    /* AP+Stationģʽ */
            break;
        }
        default:
        {
            return ATK_ESP8266D_EINVAL;
        }
    }
    
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D���û���ģʽ
 * @param       cfg: 0���رջ���
 *                   1���򿪻���
 * @retval      ATK_ESP8266D_EOK  : ���û���ģʽ�ɹ�
 *              ATK_ESP8266D_ERROR: ���û���ģʽʧ��
 */
unsigned char atk_esp8266d_ate_config(unsigned char cfg)
{
    unsigned char ret;
    
    switch (cfg)
    {
        case 0:
        {
            ret = atk_esp8266d_send_at_cmd("ATE0", "OK", 500);   /* �رջ��� */
            break;
        }
        case 1:
        {
            ret = atk_esp8266d_send_at_cmd("ATE1", "OK", 500);   /* �򿪻��� */
            break;
        }
        default:
        {
            return ATK_ESP8266D_EINVAL;
        }
    }
    
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}


//WiFi ATָ�
/*************************************************************************************************
 * @brief       ATK-ESP8266D����WIFI
 * @param       ssid: WIFI����
 *              pwd : WIFI����
 * @retval      ATK_ESP8266D_EOK  : WIFI���ӳɹ�
 *              ATK_ESP8266D_ERROR: WIFI����ʧ��
 */
unsigned char atk_esp8266d_join_ap(char *ssid, char *pwd)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    ret = atk_esp8266d_send_at_cmd(cmd, "WIFI GOT IP", 10000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}


//TCP/IP ATָ�
/*************************************************************************************************
 * @brief       ATK-ESP8266D����ʱ���� SNTP ������
 * @param       sntp: SNTP ��������ַ
 *            
 * @retval      ATK_ESP8266D_EOK  : ʹ�� SNTP �������ɹ�
 *              ATK_ESP8266D_ERROR: ʹ�� SNTP ������ʧ��
 */
unsigned char atk_esp8266d_cipsntpcfg(char *sntp)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+CIPSNTPCFG=1,8,\"%s\",\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"", sntp);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 10000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D��ȡIP��ַ
 * @param       buf: IP��ַ����Ҫ16�ֽ��ڴ�ռ�
 * @retval      ATK_ESP8266D_EOK  : ��ȡIP��ַ�ɹ�
 *              ATK_ESP8266D_ERROR: ��ȡIP��ַʧ��
 */
unsigned char atk_esp8266d_get_ip(char *buf)
{
    unsigned char ret;
    char *p_start;
    char *p_end;
    ret = atk_esp8266d_send_at_cmd("AT+CIFSR", "OK", 500);
    if (ret != ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_ERROR;
    }
    p_start = strstr((const char *)atk_esp8266d_uart_rx_get_frame(), "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    sprintf(buf, "%s", p_start + 1);
    return ATK_ESP8266D_EOK;
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D����͸��
 * @param       ��
 * @retval      ATK_ESP8266D_EOK  : ����͸���ɹ�
 *              ATK_ESP8266D_ERROR: ����͸��ʧ��
 */
unsigned char atk_esp8266d_enter_unvarnished(void)
{
    unsigned char ret;
    
    ret  = atk_esp8266d_send_at_cmd("AT+CIPMODE=1", "OK", 500);
    ret += atk_esp8266d_send_at_cmd("AT+CIPSEND", ">", 500);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D�˳�͸��
 * @param       ��
 * @retval      ��
 */
void atk_esp8266d_exit_unvarnished(void)
{
    atk_esp8266d_uart_printf("+++");
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D����TCP������
 * @param       server_ip  : TCP������IP��ַ
 *              server_port: TCP�������˿ں�
 * @retval      ATK_ESP8266D_EOK  : ����TCP�������ɹ�
 *              ATK_ESP8266D_ERROR: ����TCP������ʧ��
 */
unsigned char atk_esp8266d_connect_tcp_server(char *server_ip, char *server_port)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", server_ip, server_port);
    ret = atk_esp8266d_send_at_cmd(cmd, "CONNECT", 5000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}


//MQTT ATָ�
/*************************************************************************************************
 * @brief       ATK-ESP8266D���� MQTT ����;����ʹ�á�
 * @param       username: �û���
 *            	password: ����
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttusercfg(char *username,char *password)//���� MQTT �û�������
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"", username,password);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 5000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D���ÿͻ���ID
 * @param       clientid: �ͻ���id
 *            	
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttclientid(char *clientid)
{
    unsigned char ret;
    char cmd[CMDSIZE];
	sprintf(cmd, "AT+MQTTCLIENTID=0,\"%s\"", clientid);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 5000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D�����û���
 * @param       username: �û���
 *            	
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttusername(char *username)
{
    unsigned char ret;
    char cmd[CMDSIZE];
	sprintf(cmd, "AT+MQTTUSERNAME=0,\"%s\"", username);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 5000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D�����û�����
 * @param       password: ����
 *            	
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttpassword(char *password)
{
    unsigned char ret;
    char cmd[CMDSIZE];
	sprintf(cmd, "AT+MQTTPASSWORD=0,\"%s\"", password);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 5000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D���� MQTT ����
 * @param       host: ����
 *            	port���˿�
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttconn(char *host,char *port)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+MQTTCONN=0,\"%s\",%s,1", host,port);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 3000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ���� MQTT ��Ϣ���ַ�����
 * @param       topic: ����
 *            	strdata���ַ�����Ϣ
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttpubstr(char *topic,char *strdata)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0", topic,strdata);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 3000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ���� MQTT ��Ϣ�������ƣ�
 * @param       topic: ����
 *            	bindata����������Ϣ
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttpubraw(char *topic,char *bindata,unsigned int lenght)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+MQTTPUBRAW=0,\"%s\",%d,1", topic,lenght);
    ret = atk_esp8266d_send_at_cmd(cmd, "", 3000);
    sprintf(cmd, "\"%s\"", bindata);
    ret = atk_esp8266d_send_at_cmd(cmd, "+MQTTPUB:OK", 3000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ���� MQTT Topic
 * @param       topic: ����
 *            	
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttsub(char *topic)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+MQTTSUB=0,\"%s\",1", topic);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 3000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       ȡ������ MQTT Topic
 * @param       topic: ����
 *            	
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttunsub(char *topic)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+MQTTUNSUB=0,\"%s\"", topic);
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 3000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}

/*************************************************************************************************
 * @brief       �Ͽ� MQTT ����
 * @param       
 *            	
 * @retval      ATK_ESP8266D_EOK  : �ɹ�
 *              ATK_ESP8266D_ERROR: ʧ��
 */
unsigned char atk_esp8266d_mqttclean(void)
{
    unsigned char ret;
    char cmd[CMDSIZE];
    sprintf(cmd, "AT+MQTTCLEAN=0");
    ret = atk_esp8266d_send_at_cmd(cmd, "OK", 3000);
    if (ret == ATK_ESP8266D_EOK)
    {
        return ATK_ESP8266D_EOK;
    }
    else
    {
        return ATK_ESP8266D_ERROR;
    }
}
