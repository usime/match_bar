#include "IoTMessageHandler.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "atk_esp8266d_uart.h"
#include "cloud.h"
#include "usart2.h"

#define loTMessageDebug 1

#if loTMessageDebug
unsigned char usart_send_data[512];
#endif


char request_id[REQUEST_ID_LEN] = {0};

//云消息处理
char confirmationMqttMessage (char *mqtt_message)//确认消息是否是订阅消息 1确定，0不是
{
	char *mqttsubrecv = strstr((const char *)mqtt_message,"+MQTTSUBRECV:0");//确定接收信息为订阅信息
	if(mqttsubrecv == NULL)//不是接收MQTT消息处理
	{
		return 0;	
	}
	return 1;
}

char *getTopicSub(char *mqtt_message)//获取订阅主题
{
	char *ret;
	char *start = strchr((const char *)mqtt_message,'\"');
	if(start ==NULL)
	{
		#if loTMessageDebug
		memset(usart_send_data,0,sizeof(usart_send_data));
		sprintf((char *)usart_send_data,"Topicsub:NULL\r\n\r\n");
		UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
		#endif
		return NULL;				
	}
	start++;
	char *end = strchr((const char *)start,'\"');
	size_t length = end - start;								//不包括"
	ret = (char *)malloc(length +1); 							// 复制有效负载到新的字符串中 
	strncpy(ret, start, length);  
	ret[length] = '\0'; 										// 添加null终止符
	#if loTMessageDebug
	sprintf((char *)usart_send_data,"TopicSub:%s\r\n\r\n",ret);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif
	return ret;
}

int getDataLenght(char *mqtt_message)//获取数据长度
{
	int ret;
	char *start = strchr((const char *)mqtt_message,',');
	start++;
	start = strchr((const char *)start,',');
	if(start ==NULL)
	{
		#if loTMessageDebug
		memset(usart_send_data,0,sizeof(usart_send_data));
		sprintf((char *)usart_send_data,"datalenght:NULL\r\n\r\n");
		UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
		#endif
		return -1;				
	}
	start++;
	char *end = strchr((const char *)start,',');
	size_t length = end - start;		//不包括,
	ret = 0;							
	for (size_t i = 0; i < length; i++)//转换data_lenght为整型
	{
		ret = ret*10+(*start-0x30);
		start++;
	}
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"datalenght:%d\r\n\r\n",ret);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif
	return ret;
}

char *getJson(char *mqtt_message)//获取Json消息
{
	char *ret;
	char *start = strchr((const char *)mqtt_message,'{');
	if(start ==NULL)
	{
		#if loTMessageDebug
		memset(usart_send_data,0,sizeof(usart_send_data));
		sprintf((char *)usart_send_data,"getJson:NULL\r\n\r\n");
		UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
		#endif
		return NULL;				
	}
	char *end = strrchr((const char *)mqtt_message,'}');
	size_t length = end - start+1;								//包括{}
	ret = (char *)malloc(length +1); 							// 复制有效负载到新的字符串中 
	strncpy(ret, start, length);  
	ret[length] = '\0'; 										// 添加null终止符
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"TopicSub:%s\r\n\r\n",ret);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif
	return ret;
}

int getParamsInt(char *mqtt_message,char *identifier)//获取功能属性参数整型
{
	
	int cmd = ExtractRequestId(mqtt_message);	//获取request_id

	int ret;
	char temp[50]="\0";
	char *start = strstr((const char *)mqtt_message,"\"paras\":");	
	if(start !=NULL)
	{
		sprintf(temp,"\"%s\":",identifier);
		start = strstr((const char *)start,temp);
		if(start ==NULL)
		{
			#if loTMessageDebug
			memset(usart_send_data,0,sizeof(usart_send_data));
			sprintf((char *)usart_send_data,"<%s> is no exist\r\n",identifier);
			UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
			#endif
			return -1;				
		}		
	}
	else
	{
		#if loTMessageDebug
		memset(usart_send_data,0,sizeof(usart_send_data));
		sprintf((char *)usart_send_data,"<paras> is no exist\r\n");
		UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
		#endif
		return -1;
	}	
	start = start+strlen(temp);	
	char *end1 = strchr((const char *)start,',');
	char *end2 = strchr((const char *)start,'}');
	char *end =NULL;
	if(end1 != NULL && end2 != NULL)
	{
		if(end1>end2)end = end2;
		else end = end1;
	}
	else if(end1 != NULL && end2 == NULL)
	{
		end = end1;
	}
	else if(end1 == NULL && end2 != NULL)
	{
		end = end2;
	}
	size_t length = end - start;								//不包括"
	ret = 0;							
	for (size_t i = 0; i < length; i++)//转换data_lenght为整型
	{
		ret = ret*10+(*start-0x30);
		start++;
	}
	
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"%s:%d\r\n",identifier,ret);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif	

	if(cmd==0)
	{
		ResponseCommand();	//发布命令响应消息
	}
	
	
	return ret;	
}		

char *getParamsStr(char *mqtt_message,char *identifier)//获取功能属性参数字符串
{
	int cmd = ExtractRequestId(mqtt_message);	//获取request_id
	
	char *ret;
	char temp[128]="\0";
	char *start = strstr((const char *)mqtt_message,"\"paras\":");	
	if(start !=NULL)
	{
		sprintf(temp,"\"%s\":\"",identifier);
		start = strstr((const char *)start,temp);	
		if(start ==NULL)
		{
			#if loTMessageDebug
			memset(usart_send_data,0,sizeof(usart_send_data));
			sprintf((char *)usart_send_data,"<%s> is no exist\r\n",identifier);
			UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
			#endif
			return NULL;				
		}		
	}
	else
	{
		#if loTMessageDebug
		memset(usart_send_data,0,sizeof(usart_send_data));
		sprintf((char *)usart_send_data,"<paras> is no exist\r\n");
		UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
		#endif
		return NULL;
	}	
	start = start+strlen(temp);	
	char *end = strchr((const char *)start,'\"');
	size_t length = end - start;								//不包括"
	ret = (char *)malloc(length +1); 							// 复制有效负载到新的字符串中 
	strncpy(ret, start, length);  
	ret[length] = '\0'; 										// 添加null终止符	
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"%s:%s\r\n\r\n",identifier,ret);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif	

	if(cmd==0)
	{
		ResponseCommand();	//发布命令响应消息
	}
	
	return ret;	
}

char setParamsInt(char *topic,char *identifier,int value)//发布整型数据
{
	int ret;
	char temp[128]="\0";
	sprintf(temp,"{\\\"services\\\": [{\\\"service_id\\\": \\\"%s\\\"\\,\\\"properties\\\":{\\\"%s\\\":%d}}]}",ServiceId,identifier,value);
	ret = atk_esp8266d_mqttpubstr(topic,temp);
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"setParamsInt:%s\r\n",temp);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif
	return ret;
}

char setParamsStr(char *topic,char *identifier,char *value)//发布整型字符串数据
{
	int ret;
	
	char temp[128]="\0";
	sprintf(temp,"{\\\"services\\\": [{\\\"service_id\\\": \\\"%s\\\"\\,\\\"properties\\\":{\\\"%s\\\":%d}}]}",ServiceId,identifier,value);
	ret = atk_esp8266d_mqttpubstr(topic,temp);
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"setParamsInt:%s\r\n\r\n",temp);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif
	return ret;
}

int ExtractRequestId(char *mqtt_message)//获取request_id
{
	const char *start_marker = "request_id=";
    const char *end_marker = "\"";  // 结束符是双引号

    // 查找 "request_id=" 的位置
    char *start_pos = strstr(mqtt_message, start_marker);
    if (!start_pos)
	{
        return -1;
    }
    start_pos += strlen(start_marker);  // 跳过 "request_id="

    // 查找结束符的位置
    char *end_pos = strchr(start_pos, end_marker[0]);
    if (!end_pos)
	{
        return -1;
    }

    // 计算长度并复制到目标缓冲区
    size_t len = end_pos - start_pos;
    if (len >= REQUEST_ID_LEN)	 //request_id truncated to fit buffer
	{
        len = REQUEST_ID_LEN - 1;
    }
    strncpy(request_id, start_pos, len);
    request_id[len] = '\0';  // 确保字符串以 null 结尾
	
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"request_id:%s\r\n",request_id);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data)); 
	#endif
		
	return 0;
}

char ResponseCommand()//发布命令响应消息
{
	int ret;
	char topic[256]="\0";
	snprintf(topic,sizeof(topic),"%s%s",TopicResponse,request_id);
	#if loTMessageDebug
	memset(usart_send_data,0,sizeof(usart_send_data));
	sprintf((char *)usart_send_data,"response topic:%s\r\n",topic);
	UART_Transmit(USART2, usart_send_data, sizeof(usart_send_data));  
	#endif

	ret = atk_esp8266d_mqttpubstr(topic,"{\\\"result_code\\\": \\\"0\\\"}");

	return ret;
}
