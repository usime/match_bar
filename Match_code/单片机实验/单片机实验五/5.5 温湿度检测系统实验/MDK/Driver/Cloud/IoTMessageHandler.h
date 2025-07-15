#ifndef IoTMessageHandler_H
#define IoTMessageHandler_H


#define REQUEST_ID_LEN	64

char confirmationMqttMessage (char *mqtt_message);
char *getTopicSub(char *mqtt_message);
int getDataLenght(char *mqtt_message);
char *getJson(char *mqtt_message);
int getParamsInt(char *mqtt_message, char *identifier);
char *getParamsStr(char *mqtt_message, char *identifier);
char setParamsInt(char *topic,char *identifier,int value);
char setParamsStr(char *topic,char *identifier,char *value);


int ExtractRequestId(char *mqtt_message);
char ResponseCommand(void);
#endif
