#ifndef CLOUD_H
#define CLOUD_H
#include "atk_esp8266d.h"

#define 				waitTime 										20	
//WiFi���ƺ�����
#define 				WIFISSID									"honor"    				//�޸�
#define 				WIFIPASSWORD							"jingai200354"    				//�޸�
#define 				clientId									"685e1f97d582f20018358bbe_HardwareDevice001_0_0_2025063007"				//�޸�
#define 				username									"685e1f97d582f20018358bbe_HardwareDevice001"				//�޸�
#define 				passwd										"a6a853175c0996e888e64dab7ee95be6ccd2ffde48c6c1958c9674d7334b1262"				//�޸�
#define 				mqttHostUrl								"1e17ae9787.st1.iotda-device.cn-north-4.myhuaweicloud.com"					//�޸�
#define 				port										  "1883"						//�޸�
#define 				TopicSub									"$oc/devices/685e1f97d582f20018358bbe/sys/commands/request_id={request_id}"				//�޸�
#define 				TopicReport								"$oc/devices/685e1f97d582f20018358bbe/sys/properties/report"			//�޸�
#define					TopicResponse							"$oc/devices/685e1f97d582f20018358bbe/sys/commands/response/request_id="			//�޸�
#define 				ServiceId									"device001"					//�޸�

unsigned char Cloud_Init(void);
#endif
