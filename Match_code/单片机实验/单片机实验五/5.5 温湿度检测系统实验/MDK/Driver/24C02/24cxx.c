#include "24cxx.h" 

//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_IIC_SCK | RCC_AHBxPeriph_IIC_SDA, ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin   = IIC_SCK_GPIO_PIN;
	GPIO_Init(IIC_SCK_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(IIC_SCK_GPIO_PORT,IIC_SCK_GPIO_PIN);

	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_GPIO_PIN;
	GPIO_Init(IIC_SDA_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(IIC_SDA_GPIO_PORT,IIC_SDA_GPIO_PIN);	
	
}
//SDA�����������ģʽ
void SDA_OUT(void)
{					     
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_AHBxPeriph_IIC_SDA, ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_GPIO_PIN;
	GPIO_Init(IIC_SDA_GPIO_PORT, &GPIO_InitStructure);
}
//SDA������������ģʽ
void SDA_IN(void)
{		
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_AHBxPeriph_IIC_SDA, ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_GPIO_PIN;
	GPIO_Init(IIC_SDA_GPIO_PORT, &GPIO_InitStructure);
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_OUT = 1;	  	  
	IIC_SCK = 1;
 	IIC_SDA_OUT = 0;//START:when CLK is high,DATA change form high to low 
	IIC_SCK = 0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCK = 0;
	IIC_SDA_OUT = 0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCK = 1; 
	IIC_SDA_OUT = 1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_OUT = 1;delay_us(1);	   
	IIC_SCK = 1;delay_us(1);	 
	while(IIC_SDA_IN)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCK = 0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCK = 0;
	SDA_OUT();
	IIC_SDA_OUT = 0;
	delay_us(2);
	IIC_SCK = 1;
	delay_us(2);
	IIC_SCK = 0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCK = 0;
	SDA_OUT();
	IIC_SDA_OUT = 1;
	delay_us(2);
	IIC_SCK = 1;
	delay_us(2);
	IIC_SCK = 0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT(); 	    
    IIC_SCK = 0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_OUT = 1;
		else
			IIC_SDA_OUT = 0;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCK = 1;
		delay_us(2); 
		IIC_SCK = 0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCK = 0; 
        delay_us(2);
		IIC_SCK = 1;
        receive<<=1;
        if(IIC_SDA_IN)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}


//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
unsigned char AT24CXX_ReadOneByte(unsigned int ReadAddr)
{				  
	unsigned char temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(unsigned int WriteAddr,unsigned char DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(unsigned int WriteAddr,unsigned long DataToWrite,unsigned char Len)
{  	
	unsigned char t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
unsigned long AT24CXX_ReadLenByte(unsigned int ReadAddr,unsigned char Len)
{  	
	unsigned char t;
	unsigned long temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
unsigned char AT24CXX_Check(void)
{
	unsigned char temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(unsigned int ReadAddr,unsigned char *pBuffer,unsigned int NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(unsigned int WriteAddr,unsigned char *pBuffer,unsigned int NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
 











