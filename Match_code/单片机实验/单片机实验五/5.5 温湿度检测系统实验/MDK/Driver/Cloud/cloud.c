
#include "cloud.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>

unsigned char Cloud_Init(void)
{
	unsigned char ret =0;
    ret = atk_esp8266d_init(115200);
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
    ret = atk_esp8266d_sw_reset();
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
	ret = atk_esp8266d_at_test();
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
    ret = atk_esp8266d_set_mode(1);
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
	ret = atk_esp8266d_join_ap(WIFISSID,WIFIPASSWORD);
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
	ret = atk_esp8266d_mqttusercfg(username,passwd);
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
	ret = atk_esp8266d_mqttclientid(clientId);
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
	ret = atk_esp8266d_mqttconn(mqttHostUrl,port);
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
	ret = atk_esp8266d_mqttsub(TopicSub);
    if (ret != 0)
    {
       return ret;
    }
	delay_ms(waitTime);
	return ret;
}
