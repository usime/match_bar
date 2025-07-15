
#ifndef __ATK_ESP8266D_H
#define __ATK_ESP8266D_H

#include "sys.h"
#include "atk_esp8266d_uart.h"


#define atk_esp8266d_delay_ms	delay_ms 	//延时函数重定义
#define CMDSIZE 256							/* AT指令最大支持256字节 */


// ?  AT+MQTTUSERCFG：  设置 MQTT 用户属性
// ?  AT+MQTTCLIENTID： 设置 MQTT 客户端 ID
// ?  AT+MQTTUSERNAME： 设置 MQTT 登陆用户名
// ?  AT+MQTTPASSWORD： 设置 MQTT 登陆密码
// ?  AT+MQTTCONNCFG：  设置 MQTT 连接属性
// ?  AT+MQTTCONN：     连接 MQTT Broker
// ?  AT+MQTTPUB：      发布 MQTT 消息（字符串）
// ?  AT+MQTTPUBRAW：   发布 MQTT 消息（二进制）
// ?  AT+MQTTSUB：      订阅 MQTT Topic
// ?  AT+MQTTUNSUB：    取消订阅 MQTT Topic
// ?  AT+MQTTCLEAN：    断开 MQTT 连接 


/* 错误代码 */
#define ATK_ESP8266D_EOK         0   /* 没有错误 */
#define ATK_ESP8266D_ERROR       1   /* 通用错误 */
#define ATK_ESP8266D_ETIMEOUT    2   /* 超时错误 */
#define ATK_ESP8266D_EINVAL      3   /* 参数错误 */

/* 操作函数 */
unsigned char atk_esp8266d_send_at_cmd(char *cmd, char *ack, uint32_t timeout);    /* 发送AT指令 */

unsigned char atk_esp8266d_init(uint32_t baudrate);                                /* 初始化 */
unsigned char atk_esp8266d_at_test(void);                                          /*  AT指令测试 */
unsigned char atk_esp8266d_restore(void);                                          /* 恢复出厂设置 */
unsigned char atk_esp8266d_sw_reset(void);                                         /* 软件复位 */
unsigned char atk_esp8266d_set_mode(unsigned char mode);                           /* 设置工作模式 */
unsigned char atk_esp8266d_ate_config(unsigned char cfg);                          /* 设置回显模式 */


unsigned char atk_esp8266d_join_ap(char *ssid, char *pwd);                         /* 连接WIFI */
unsigned char atk_esp8266d_cipsntpcfg(char *sntp);								   /* 设置时区和 SNTP 服务器 */
unsigned char atk_esp8266d_get_ip(char *buf);                                      /* 获取IP地址 */
unsigned char atk_esp8266d_enter_unvarnished(void);                                /* 进入透传 */
void atk_esp8266d_exit_unvarnished(void);                                   	/* 退出透传 */
unsigned char atk_esp8266d_connect_tcp_server(char *server_ip, char *server_port); /* 连接TCP服务器 */


unsigned char atk_esp8266d_mqttusercfg(char *username,char *password);				/* 设置 MQTT 用户属性 */
unsigned char atk_esp8266d_mqttclientid(char *clientid);							/* 设置客户端ID */
unsigned char atk_esp8266d_mqttusername(char *username);                           /* 设置用户名 */
unsigned char atk_esp8266d_mqttpassword(char *password);                           /* 设置用户密码 */
unsigned char atk_esp8266d_mqttconn(char *host, char *port);                       /* 设置 MQTT 连接 */
unsigned char atk_esp8266d_mqttpubstr(char *topic, char *strdata);                 /* 发布 MQTT 消息（字符串） */
unsigned char atk_esp8266d_mqttpubraw(char *topic, char *bindata, unsigned int lenght);/* 发布 MQTT 消息（二进制） */
unsigned char atk_esp8266d_mqttsub(char *topic);                                   /* 订阅 MQTT Topic */
unsigned char atk_esp8266d_mqttunsub(char *topic);                                 /* 取消订阅 Topic*/
unsigned char atk_esp8266d_mqttclean(void);                                        /* 断开 MQTT 连接 */

#endif
