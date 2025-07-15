#include "dht11.h"

DHT11_TypeDef DHT11;					      	//全局变量
	//DHT11的IO初始化
void DHT_OUTPUT_PP(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_DHT, ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = DHT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT_GPIO_PORT, &GPIO_InitStructure); 			//初始化GPIO
}
void DHT_INPUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_DHT, ENABLE);		//使能端口时钟
	GPIO_InitStructure.GPIO_Pin   = DHT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT_GPIO_PORT, &GPIO_InitStructure); 			//初始化GPIO	
}

//DHT11初始化
void DHT11_init(void)
{
		DHT_OUTPUT_PP();
	 	DHT11.Tem_H = 0;
		DHT11.Tem_L = 0;
		DHT11.Hum_H = 0;
		DHT11.Hum_L = 0; 		
}

//读取温湿度一位
static char dht11_read_bit(void)
{
 	uint8_t retry=0;
	while(DHT_READ_IO&&retry<100)//等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT_READ_IO&&retry<100)//等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DHT_READ_IO)
		return 1;
	else 
		return 0;	
}

//读取温湿度一个字节
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


//读取温湿度
int DHT11_ReadData(void)
{
	uint16_t cout = 0;
	unsigned int T_H, T_L, H_H, H_L, Check;

	//设置为IO口输出模式
	DHT_OUTPUT_PP();
	DHT_WRITE_IO=1;
	//MCU开始起始信号
	DHT_WRITE_IO=0;
	delay_ms(20);		//拉低至少18ms
	//DHT_WRITE_IO=1;		
	//设置为IO口输入模式
	DHT_INPUT();
	
	//检测应答信号
	cout=0;
	while(DHT_READ_IO && ++cout);
	if (cout == 0) return 0;
	
	//总线由上拉电阻拉高 主机延时20us
	//主机设为输入 判断从机响应信号  
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  	 	
	cout=0;
	while(!DHT_READ_IO && ++cout);
	if (cout == 0) return 0;	
	cout=0;
	while(DHT_READ_IO && ++cout);
	if (cout == 0) return 0;
	
	//读取8bit的湿度整数数据
	H_H = dht11_read_byte();
	//读取8bit的湿度小数数据
	H_L = dht11_read_byte();
	//读取8bit的温度整数数据
	T_H = dht11_read_byte();
	//读取8bit的温度小数数据
	T_L = dht11_read_byte();
	//读取8bit的校验和
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

