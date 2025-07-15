
#ifndef __ATK_ESP8266D_H
#define __ATK_ESP8266D_H

#include "sys.h"
#include "atk_esp8266d_uart.h"


#define atk_esp8266d_delay_ms	delay_ms 	//��ʱ�����ض���
#define CMDSIZE 256							/* ATָ�����֧��256�ֽ� */


// ?  AT+MQTTUSERCFG��  ���� MQTT �û�����
// ?  AT+MQTTCLIENTID�� ���� MQTT �ͻ��� ID
// ?  AT+MQTTUSERNAME�� ���� MQTT ��½�û���
// ?  AT+MQTTPASSWORD�� ���� MQTT ��½����
// ?  AT+MQTTCONNCFG��  ���� MQTT ��������
// ?  AT+MQTTCONN��     ���� MQTT Broker
// ?  AT+MQTTPUB��      ���� MQTT ��Ϣ���ַ�����
// ?  AT+MQTTPUBRAW��   ���� MQTT ��Ϣ�������ƣ�
// ?  AT+MQTTSUB��      ���� MQTT Topic
// ?  AT+MQTTUNSUB��    ȡ������ MQTT Topic
// ?  AT+MQTTCLEAN��    �Ͽ� MQTT ���� 


/* ������� */
#define ATK_ESP8266D_EOK         0   /* û�д��� */
#define ATK_ESP8266D_ERROR       1   /* ͨ�ô��� */
#define ATK_ESP8266D_ETIMEOUT    2   /* ��ʱ���� */
#define ATK_ESP8266D_EINVAL      3   /* �������� */

/* �������� */
unsigned char atk_esp8266d_send_at_cmd(char *cmd, char *ack, uint32_t timeout);    /* ����ATָ�� */

unsigned char atk_esp8266d_init(uint32_t baudrate);                                /* ��ʼ�� */
unsigned char atk_esp8266d_at_test(void);                                          /*  ATָ����� */
unsigned char atk_esp8266d_restore(void);                                          /* �ָ��������� */
unsigned char atk_esp8266d_sw_reset(void);                                         /* �����λ */
unsigned char atk_esp8266d_set_mode(unsigned char mode);                           /* ���ù���ģʽ */
unsigned char atk_esp8266d_ate_config(unsigned char cfg);                          /* ���û���ģʽ */


unsigned char atk_esp8266d_join_ap(char *ssid, char *pwd);                         /* ����WIFI */
unsigned char atk_esp8266d_cipsntpcfg(char *sntp);								   /* ����ʱ���� SNTP ������ */
unsigned char atk_esp8266d_get_ip(char *buf);                                      /* ��ȡIP��ַ */
unsigned char atk_esp8266d_enter_unvarnished(void);                                /* ����͸�� */
void atk_esp8266d_exit_unvarnished(void);                                   	/* �˳�͸�� */
unsigned char atk_esp8266d_connect_tcp_server(char *server_ip, char *server_port); /* ����TCP������ */


unsigned char atk_esp8266d_mqttusercfg(char *username,char *password);				/* ���� MQTT �û����� */
unsigned char atk_esp8266d_mqttclientid(char *clientid);							/* ���ÿͻ���ID */
unsigned char atk_esp8266d_mqttusername(char *username);                           /* �����û��� */
unsigned char atk_esp8266d_mqttpassword(char *password);                           /* �����û����� */
unsigned char atk_esp8266d_mqttconn(char *host, char *port);                       /* ���� MQTT ���� */
unsigned char atk_esp8266d_mqttpubstr(char *topic, char *strdata);                 /* ���� MQTT ��Ϣ���ַ����� */
unsigned char atk_esp8266d_mqttpubraw(char *topic, char *bindata, unsigned int lenght);/* ���� MQTT ��Ϣ�������ƣ� */
unsigned char atk_esp8266d_mqttsub(char *topic);                                   /* ���� MQTT Topic */
unsigned char atk_esp8266d_mqttunsub(char *topic);                                 /* ȡ������ Topic*/
unsigned char atk_esp8266d_mqttclean(void);                                        /* �Ͽ� MQTT ���� */

#endif
