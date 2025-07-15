//头文件
#include "seg.h"

u8 const DISP_TAB[] = {
    0xc0,
    0xf9,
    0xa4,
    0xb0,
    0x99,
    0x92,
    0x82,
    0xf8,
    0x80,
    0x90,
    0x88,
    0x83,
    0xc6,
    0xa1,
    0x86,
    0x8e,
    0x8c,
    0xc1,
    0x91,
    0x7c,
    0x00,
    0xff,
    0x7f
};

/*******************************************************************************
GPIO初始化
*******************************************************************************/
void SEG_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_SEG_CS , ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = SEG_CS_GPIO_PIN;
	GPIO_Init(SEG_CS_GPIO_PORT,&GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_SEG_SCK , ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = SEG_SCK_GPIO_PIN;
	GPIO_Init(SEG_SCK_GPIO_PORT,&GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_SEG_MOSI , ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = SEG_MOSI_GPIO_PIN;
	GPIO_Init(SEG_MOSI_GPIO_PORT,&GPIO_InitStructure);
}

unsigned char SPI_ReadWrite(unsigned char duan,unsigned char wei)
{
	unsigned char i=0;
	unsigned char ReData=0;
	SEG_CS = 0;
	for(i=8;i>0;i--)
	{
	  if(wei&0x80)	
		  SEG_MOSI =1; //输出数据
      else 
		  SEG_MOSI =0;
      SEG_SCK = 0;       
      SEG_SCK = 1;
      wei <<= 1; 
	}
	for(i=8;i>0;i--)
	{
	  if(duan&0x80)	
		  SEG_MOSI =1; //输出数据
      else 
		  SEG_MOSI =0;
      SEG_SCK = 0;       
      SEG_SCK = 1;
      duan <<= 1; 
	}
	SEG_CS = 1;
	return ReData;
}

//共阴数码管显示
void Write_Seg(u8 num,u8 com)
{
	SPI_ReadWrite(~DISP_TAB[num],~(1<<com));
}


