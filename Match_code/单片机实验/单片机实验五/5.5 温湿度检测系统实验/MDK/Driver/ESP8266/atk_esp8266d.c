//本文件是AT指令集发送和返回数据匹配
//ATK_ESP8266D_EOK：成功
//ATK_ESP8266D_ERROR：失败

#include "atk_esp8266d.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*************************************************************************************************
 * @brief       ATK-ESP8266D发送AT指令
 * @param       cmd    : 待发送的AT指令
 *              ack    : 等待的响应
 *              timeout: 等待超时时间
 * @retval      ATK_ESP8266D_EOK     : 函数执行成功
 *              ATK_ESP8266D_ETIMEOUT: 等待期望应答超时，函数执行失败
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
 * @brief       ATK-ESP8266D初始化
 * @param       baudrate: ATK-ESP8266D UART通讯波特率
 * @retval      ATK_ESP8266D_EOK  : ATK-ESP8266D初始化成功，函数执行成功
 *              ATK_ESP8266D_ERROR: ATK-ESP8266D初始化失败，函数执行失败
 */
unsigned char atk_esp8266d_init(uint32_t baudrate)
{
    atk_esp8266d_uart_init(baudrate);                /* ATK-ESP8266D UART初始化 */
    if (atk_esp8266d_at_test() != ATK_ESP8266D_EOK)   /* ATK-ESP8266D AT指令测试 */
    {
        return ATK_ESP8266D_ERROR;
    }
    
    return ATK_ESP8266D_EOK;
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D AT指令测试
 * @param       无
 * @retval      ATK_ESP8266D_EOK  : AT指令测试成功
 *              ATK_ESP8266D_ERROR: AT指令测试失败
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


//基础AT指令集
/*************************************************************************************************
 * @brief       ATK-ESP8266D恢复出厂设置
 * @param       无
 * @retval      ATK_ESP8266D_EOK  : 恢复出场设置成功
 *              ATK_ESP8266D_ERROR: 恢复出场设置失败
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
 * @brief       ATK-ESP8266D软件复位
 * @param       无
 * @retval      ATK_ESP8266D_EOK  : 软件复位成功
 *              ATK_ESP8266D_ERROR: 软件复位失败
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
 * @brief       设置ATK-ESP8266D工作模式
 * @param       mode: 1，Station模式
 *                    2，AP模式
 *                    3，AP+Station模式
 * @retval      ATK_ESP8266D_EOK   : 工作模式设置成功
 *              ATK_ESP8266D_ERROR : 工作模式设置失败
 *              ATK_ESP8266D_EINVAL: mode参数错误，工作模式设置失败
 */
unsigned char atk_esp8266d_set_mode(unsigned char mode)
{
    unsigned char ret;
    switch (mode)
    {
        case 1:
        {
            ret = atk_esp8266d_send_at_cmd("AT+CWMODE=1", "OK", 500);    /* Station模式 */
            break;
        }
        case 2:
        {
            ret = atk_esp8266d_send_at_cmd("AT+CWMODE=2", "OK", 500);    /* AP模式 */
            break;
        }
        case 3:
        {
            ret = atk_esp8266d_send_at_cmd("AT+CWMODE=3", "OK", 500);    /* AP+Station模式 */
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
 * @brief       ATK-ESP8266D设置回显模式
 * @param       cfg: 0，关闭回显
 *                   1，打开回显
 * @retval      ATK_ESP8266D_EOK  : 设置回显模式成功
 *              ATK_ESP8266D_ERROR: 设置回显模式失败
 */
unsigned char atk_esp8266d_ate_config(unsigned char cfg)
{
    unsigned char ret;
    
    switch (cfg)
    {
        case 0:
        {
            ret = atk_esp8266d_send_at_cmd("ATE0", "OK", 500);   /* 关闭回显 */
            break;
        }
        case 1:
        {
            ret = atk_esp8266d_send_at_cmd("ATE1", "OK", 500);   /* 打开回显 */
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


//WiFi AT指令集
/*************************************************************************************************
 * @brief       ATK-ESP8266D连接WIFI
 * @param       ssid: WIFI名称
 *              pwd : WIFI密码
 * @retval      ATK_ESP8266D_EOK  : WIFI连接成功
 *              ATK_ESP8266D_ERROR: WIFI连接失败
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


//TCP/IP AT指令集
/*************************************************************************************************
 * @brief       ATK-ESP8266D设置时区和 SNTP 服务器
 * @param       sntp: SNTP 服务器地址
 *            
 * @retval      ATK_ESP8266D_EOK  : 使能 SNTP 服务器成功
 *              ATK_ESP8266D_ERROR: 使能 SNTP 服务器失败
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
 * @brief       ATK-ESP8266D获取IP地址
 * @param       buf: IP地址，需要16字节内存空间
 * @retval      ATK_ESP8266D_EOK  : 获取IP地址成功
 *              ATK_ESP8266D_ERROR: 获取IP地址失败
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
 * @brief       ATK-ESP8266D进入透传
 * @param       无
 * @retval      ATK_ESP8266D_EOK  : 进入透传成功
 *              ATK_ESP8266D_ERROR: 进入透传失败
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
 * @brief       ATK-ESP8266D退出透传
 * @param       无
 * @retval      无
 */
void atk_esp8266d_exit_unvarnished(void)
{
    atk_esp8266d_uart_printf("+++");
}

/*************************************************************************************************
 * @brief       ATK-ESP8266D连接TCP服务器
 * @param       server_ip  : TCP服务器IP地址
 *              server_port: TCP服务器端口号
 * @retval      ATK_ESP8266D_EOK  : 连接TCP服务器成功
 *              ATK_ESP8266D_ERROR: 连接TCP服务器失败
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


//MQTT AT指令集
/*************************************************************************************************
 * @brief       ATK-ESP8266D设置 MQTT 配置;必须使用。
 * @param       username: 用户名
 *            	password: 密码
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
 */
unsigned char atk_esp8266d_mqttusercfg(char *username,char *password)//设置 MQTT 用户名密码
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
 * @brief       ATK-ESP8266D设置客户端ID
 * @param       clientid: 客户端id
 *            	
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       ATK-ESP8266D设置用户名
 * @param       username: 用户名
 *            	
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       ATK-ESP8266D设置用户密码
 * @param       password: 密码
 *            	
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       ATK-ESP8266D设置 MQTT 连接
 * @param       host: 主机
 *            	port：端口
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       发布 MQTT 消息（字符串）
 * @param       topic: 主题
 *            	strdata：字符串消息
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       发布 MQTT 消息（二进制）
 * @param       topic: 主题
 *            	bindata：二进制消息
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       订阅 MQTT Topic
 * @param       topic: 主题
 *            	
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       取消订阅 MQTT Topic
 * @param       topic: 主题
 *            	
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
 * @brief       断开 MQTT 连接
 * @param       
 *            	
 * @retval      ATK_ESP8266D_EOK  : 成功
 *              ATK_ESP8266D_ERROR: 失败
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
