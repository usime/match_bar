#ifndef  __DHT11_H__
#define  __DHT11_H__

#include <sys.h>

/* 定义 DHT11 引脚 */
#define 	RCC_AHBxPeriph_DHT		RCC_APB2Periph_GPIOB
#define 	DHT_GPIO_PORT			GPIOB
#define 	DHT_GPIO_PIN			GPIO_Pin_3
#define 	DHT_WRITE_IO			PBout(3)
#define 	DHT_READ_IO				PBin(3)

typedef struct _DHT11
{
	char Tem_H;		//温度整数部分
	char Tem_L;		//温度小数部分
	char Hum_H;		//湿度整数部分
	char Hum_L;		//湿度小数部分
	
}DHT11_TypeDef;

extern DHT11_TypeDef DHT11;
void DHT11_init(void);
int DHT11_ReadData(void);
#endif


