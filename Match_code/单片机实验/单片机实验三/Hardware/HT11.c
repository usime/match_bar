#include "HT11.h"
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
DHT11_TypeDef DHT11;					      	//ȫ�ֱ���
	//DHT11��IO��ʼ��
void DHT_OUTPUT_PP(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_DHT, ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = DHT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT_GPIO_PORT, &GPIO_InitStructure); 			//��ʼ��GPIO
}
void DHT_INPUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_DHT, ENABLE);		//ʹ�ܶ˿�ʱ��
	GPIO_InitStructure.GPIO_Pin   = DHT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT_GPIO_PORT, &GPIO_InitStructure); 			//��ʼ��GPIO	
}

//DHT11��ʼ��
void DHT11_init(void)
{
		DHT_OUTPUT_PP();
	 	DHT11.Tem_H = 0;
		DHT11.Tem_L = 0;
		DHT11.Hum_H = 0;
		DHT11.Hum_L = 0; 		
}

//��ȡ��ʪ��һλ
static char dht11_read_bit(void)
{
 	uint8_t retry=0;
	while(DHT_READ_IO&&retry<100)//�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!DHT_READ_IO&&retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//�ȴ�40us
	if(DHT_READ_IO)
		return 1;
	else 
		return 0;	
}

//��ȡ��ʪ��һ���ֽ�
static unsigned char dht11_read_byte(void)
{
    uint8_t i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=dht11_read_bit();
    }						    
    return dat;
}


//��ȡ��ʪ��
int DHT11_ReadData(void)
{
	uint16_t cout = 0;
	unsigned int T_H, T_L, H_H, H_L, Check;
	//����ΪIO�����ģʽ
	DHT_OUTPUT_PP();
	DHT_WRITE_IO=1;
	//MCU��ʼ��ʼ�ź�
	DHT_WRITE_IO=0;
	Delay_ms(20);		//��������18ms
	//DHT_WRITE_IO=1;		
	//����ΪIO������ģʽ
	DHT_INPUT();
	cout=0;
	while(DHT_READ_IO && ++cout);
	if (cout == 0) return 0; 	
	cout=0;
	while(!DHT_READ_IO && ++cout);
	if (cout == 0) return 0;	
	cout=0;
	while(DHT_READ_IO && ++cout);
	if (cout == 0) return 0;
	//��ȡ8bit��ʪ����������
	H_H = dht11_read_byte();
	//��ȡ8bit��ʪ��С������
	H_L = dht11_read_byte();
	//��ȡ8bit���¶���������
	T_H = dht11_read_byte();
	//��ȡ8bit���¶�С������
	T_L = dht11_read_byte();
	//��ȡ8bit��У���
	Check = dht11_read_byte();
	if(Check == (H_H + H_L + T_H + T_L))
	{
		DHT11.Hum_H = H_H;
		DHT11.Hum_L = H_L;
		DHT11.Tem_H = T_H;
		DHT11.Tem_L = T_L;	
		return 1;
	}
	else
		return 0;
}

