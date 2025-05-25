
#ifndef  __OLED_H__
#define  __OLED_H__

#include <stm32f10x.h>
#include <sys.h>

//-----------------OLED IIC端口定义----------------  
#define 	RCC_AHBxPeriph_OLED_SCLK    RCC_APB2Periph_GPIOB
#define 	OLED_SCLK_GPIO_PORT      	GPIOB
#define  	OLED_SCLK_GPIO_PIN   		GPIO_Pin_6
#define 	OLED_SCLK 					PBout(6)

#define 	RCC_AHBxPeriph_OLED_SDIN    RCC_APB2Periph_GPIOB
#define  	OLED_SDIN_GPIO_PORT       	GPIOB
#define  	OLED_SDIN_GPIO_PIN   		GPIO_Pin_7
#define		OLED_SDAO 					PBout(7)

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	
		    
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED控制用函数
void Delay_1ms(unsigned int Del_1ms);
void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	
void OLED_ShowNum(unsigned char x,unsigned char y,u32 num,unsigned char len,unsigned char size);


void OLED_Init(void);																						//OLED液晶初始化
void OLED_Clear(void);																						//OLED液晶清除显示
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t) ;										//OLED液晶画点
void OLED_DrawLine(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char mode);	//OLED液晶画线保留数据
void OLED_DrawLine1(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char mode);//OLED液晶画线不保留数据
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size);				//OLED液晶显示单个字符
void OLED_ShowString(unsigned char x,unsigned char y,char *chr,unsigned char Char_Size);	 				//OLED液晶显示字符串
void OLED_Set_Pos(unsigned char x, unsigned char y);														//OLED液晶坐标设置
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char *c);									//OLED液晶显示单个汉字
void LCD_write_hzstring(unsigned char x,unsigned char y,unsigned char *c,unsigned char num);				//OLED液晶显示汉字串
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);//OLED液晶显示图片数组
void OLED_DrawBMP1(void);																					//OLED液晶显示图片数组						
void OLED_Fill_Picture(unsigned char fill_Data);															//OLED液晶显示图片数组
void OLED_Show_Str(unsigned int x, unsigned int y,unsigned char *str);										//OLED显示数据，支持汉字和字符。默认显示格式16*16汉字或8*16字符

//IIC函数
void OLED_IIC_Start(void);
void OLED_IIC_Stop(void);
void OLED_Write_IIC_Command(unsigned char IIC_Command);
void OLED_Write_IIC_Data(unsigned char IIC_Data);
void OLED_Write_IIC_Byte(unsigned char IIC_Byte);
void OLED_IIC_Wait_Ack(void);






#endif


