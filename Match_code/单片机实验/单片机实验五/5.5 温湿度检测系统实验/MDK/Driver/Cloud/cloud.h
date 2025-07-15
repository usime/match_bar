#ifndef CLOUD_H
#define CLOUD_H
#include "atk_esp8266d.h"

#define 				waitTime 										20	
//WiFi名称和密码
#define 				WIFISSID									"honor"    				//修改
#define 				WIFIPASSWORD							"jingai200354"    				//修改
#define 				clientId									"685e1f97d582f20018358bbe_HardwareDevice001_0_0_2025063007"				//修改
#define 				username									"685e1f97d582f20018358bbe_HardwareDevice001"				//修改
#define 				passwd										"a6a853175c0996e888e64dab7ee95be6ccd2ffde48c6c1958c9674d7334b1262"				//修改
#define 				mqttHostUrl								"1e17ae9787.st1.iotda-device.cn-north-4.myhuaweicloud.com"					//修改
#define 				port										  "1883"						//修改
#define 				TopicSub									"$oc/devices/685e1f97d582f20018358bbe/sys/commands/request_id={request_id}"				//修改
#define 				TopicReport								"$oc/devices/685e1f97d582f20018358bbe/sys/properties/report"			//修改
#define					TopicResponse							"$oc/devices/685e1f97d582f20018358bbe/sys/commands/response/request_id="			//修改
#define 				ServiceId									"device001"					//修改

unsigned char Cloud_Init(void);
#endif
