#include "OLED.h"
#include "oledfont.h" 

uint8_t OLED_GRAM[144][8];

//IIC����
void OLED_IIC_Start(void)
{
	OLED_SCLK=1 ;
	OLED_SDAO=1;
	OLED_SDAO=0;
	OLED_SCLK=0;
}

//IICֹͣ
void OLED_IIC_Stop(void)
{
	OLED_SCLK=1 ;
	OLED_SDAO=0;
	OLED_SDAO=1;
}

//IIC�ȴ�Ӧ��
void OLED_IIC_Wait_Ack(void)
{
	OLED_SCLK=1 ;
	OLED_SCLK=0;
}

//IICдһ���ַ�
void OLED_Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK=0;
	for(i=0;i<8;i++)		
	{
		m=da;
		m=m&0x80;
		if(m==0x80)
		{OLED_SDAO=1;}
		else OLED_SDAO=0;
			da=da<<1;
		OLED_SCLK=1;
		OLED_SCLK=0;
	}
}

//Һ��д����
void OLED_Write_IIC_Command(unsigned char IIC_Command)
{
	OLED_IIC_Start();
	OLED_Write_IIC_Byte(0x78);          
	OLED_IIC_Wait_Ack();	
	OLED_Write_IIC_Byte(0x00);			
	OLED_IIC_Wait_Ack();	
	OLED_Write_IIC_Byte(IIC_Command); 
	OLED_IIC_Wait_Ack();	
	OLED_IIC_Stop();
}

//Һ��д����
void OLED_Write_IIC_Data(unsigned char IIC_Data)
{
	OLED_IIC_Start();
	OLED_Write_IIC_Byte(0x78);		
	OLED_IIC_Wait_Ack();	
	OLED_Write_IIC_Byte(0x40);			
	OLED_IIC_Wait_Ack();	
	OLED_Write_IIC_Byte(IIC_Data);
	OLED_IIC_Wait_Ack();	
	OLED_IIC_Stop();
}

//ѡ��Һ��д���ݻ�������
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		OLED_Write_IIC_Data(dat);
	}
	else 
	{
		OLED_Write_IIC_Command(dat);
	}
}

//Һ����ͼ
void OLED_Fill_Picture(unsigned char Fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(Fill_Data,1);
		}
	}
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}

//��������
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}

//Description :  ����OLED��ʾ   
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//Description :  �ر�OLED��ʾ 
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		 

//Description : ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	   
void OLED_Clear(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //������ʾ
}

void OLED_On(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //������ʾ
}


//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���	
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t)  
{
	unsigned char i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
	OLED_Set_Pos(x,i);
	OLED_WR_Byte(OLED_GRAM[x][i],OLED_DATA);
}

void OLED_DrawPoint1(unsigned char x,unsigned char y,unsigned char t)  
{
	unsigned char i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}

//����
//x1,y1:�������
//x2,y2:��������
void OLED_DrawLine(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char mode)
{
	unsigned int t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

void OLED_DrawLine1(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char mode)
{
	unsigned int t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//����
		OLED_DrawPoint1(uRow,uCol,!mode);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//�õ�ƫ�ƺ��ֵ			
	if(x>Max_Column-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
	else 
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WR_Byte(F6x8[c][i],OLED_DATA);
		
	}
}

//Description :  m^n����
u32 OLED_Pow(unsigned char m,unsigned char n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  

//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(unsigned char x,unsigned char y,u32 num,unsigned char len,unsigned char size2)
{         	
	unsigned char t,temp;
	unsigned char enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 

//��ʾһ���ַ��Ŵ�
void OLED_ShowString(unsigned char x,unsigned char y,char *chr,unsigned char Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120){x=0;y+=2;}
		j++;
	}
}
//��ʾ����
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char *c)
{      			    
	unsigned char t,adder=0;
	unsigned char no;
	for(no=0;no<hz_leght;no++)  //�麺��λ��
	{
		//	if(*c==hz[hz_c][0]&&*(c+1)==hz[hz_c][1])break;
		if(*c==hz[no]&&*(c+1)==hz[no+1])break; 
		no++;
	}
	no=no/2;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
	{
		//OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
		OLED_WR_Byte(Hzk[no][t],OLED_DATA);
		adder+=1;
	}	
	OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
	{	
		//OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		OLED_WR_Byte(Hzk[no][16+t],OLED_DATA);
		adder+=1;
	}					
}

//X--�У�0--3��  Y�У�0--15��  c�������ַ����� num�����ָ�����-----------------
void LCD_write_hzstring(unsigned char x,unsigned char y,unsigned char *c,unsigned char num)
{
	while(*c)
	{
		OLED_ShowCHinese(x,y,c);
		c++;
		c++;
		x=x+16;
		//x++;
		num--;
		if(num==0)break;
	}
}
//������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x1;x++)
		{      
			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
		}
	}
} 

void OLED_DrawBMP1(void)
{ 	
	unsigned char x,y; 
	for(y=0;y<8;y++)
	{
		OLED_Set_Pos(0,y);
		for(x=0;x<70;x++)
	    {      
	    	OLED_WR_Byte(OLED_GRAM[x][y],OLED_DATA);	    	
	    }

	}
} 


//��ʾ
void OLED_Show_Str(unsigned int x, unsigned int y,unsigned char *str)
{										  	  
	unsigned char bHz=0;     //�ַ��������� 
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {  
	        if(*str>0x80)bHz=1;	//���� 
	        else              	//�ַ�
	        {          
				OLED_ShowChar(x,y,*str,16);
				x+=8;  
				str++;   
			}
        }
		else//���� 
        {   
					bHz=0;//�к��ֿ�    
					OLED_ShowCHinese(x,y,str);
					str+=2; 
					x+=16;//��һ������ƫ��	    
        }						 
    }   
}

//��ʼ��SSD1306				    
void OLED_Init(void)
{ 	 	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_OLED_SCLK | RCC_AHBxPeriph_OLED_SDIN, ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin   = OLED_SCLK_GPIO_PIN;
	GPIO_Init(OLED_SCLK_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(OLED_SCLK_GPIO_PORT,OLED_SCLK_GPIO_PIN);

	GPIO_InitStructure.GPIO_Pin   = OLED_SDIN_GPIO_PIN;
	GPIO_Init(OLED_SDIN_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(OLED_SDIN_GPIO_PORT,OLED_SDIN_GPIO_PIN);	
	
	Delay_1ms(300);
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}  


