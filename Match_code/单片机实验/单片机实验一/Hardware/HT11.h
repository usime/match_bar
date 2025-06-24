#ifndef  __HT11_H__
#define  __HT11_H__

#include "sys.h"
/* ���� DHT11 ���� */
#define 	RCC_AHBxPeriph_DHT		RCC_APB2Periph_GPIOB
#define 	DHT_GPIO_PORT			GPIOB
#define 	DHT_GPIO_PIN			GPIO_Pin_3
#define 	DHT_WRITE_IO			PBout(3)
#define 	DHT_READ_IO				PBin(3)

typedef struct _DHT11
{
	char Tem_H;		
	char Tem_L;		
	char Hum_H;		
	char Hum_L;		
	
}DHT11_TypeDef;

extern DHT11_TypeDef DHT11;
void DHT11_init(void);
int DHT11_ReadData(void);
#endif


