#include "HeaderFiles.h"
#include "MySD.h"
#include "Function.h"
#include "LED.h"
#include "RTC.h"
#include "Serial.h"
#include "SPI_FLASH.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "ADC.h"

/************************* �궨�� *************************/
#define  SFLASH_ID                     0xC84013
#define BUFFER_SIZE                    256
#define TX_BUFFER_SIZE                 BUFFER_SIZE
#define RX_BUFFER_SIZE                 BUFFER_SIZE
#define  FLASH_WRITE_ADDRESS           0x000000
#define  FLASH_READ_ADDRESS            FLASH_WRITE_ADDRESS

/************************ �������� ************************/
uint32_t flash_id = 0;
uint8_t  tx_buffer[TX_BUFFER_SIZE];
uint8_t  rx_buffer[TX_BUFFER_SIZE];
uint16_t i = 0, count, result = 0;
uint8_t  is_successful = 0;
extern int Adc_value;    // ADC����������
extern float Vol_Value;

FIL fdst;
FATFS fs;
UINT br, bw;
BYTE buffer[128];
BYTE filebuffer[128];


void InputSD(float Vol_Value)
{
	nvic_config();		//�����жϿ�����
	uint16_t k = 5;
	DSTATUS stat = 0;

	do
	{
		stat = disk_initialize(0); 			//��ʼ��SD�����豸��0��,�������������,ÿ����������������Ӳ�̡�U �̵ȣ�ͨ����������һ��Ψһ�ı�š�
	}while((stat != 0) && (--k));			//�����ʼ��ʧ�ܣ��������k�Ρ�
    
    printf("SD Card disk_initialize:%d\r\n",stat);
    f_mount(0, &fs);						 //����SD�����ļ�ϵͳ���豸��0����
    printf("SD Card f_mount:%d\r\n",stat);

	if(RES_OK == stat)						 //���ع��ؽ����FR_OK ��ʾ�ɹ�����
	{        
        printf("\r\nSD Card Initialize Success!\r\n");
		
		result = f_open(&fdst, "0:/MyFile.TXT", FA_WRITE | FA_OPEN_ALWAYS);		//��SD���ϴ����ļ�FATFS.TXT��
		
		if(result != FR_OK) {
            printf("f_open failed: %d\r\n", result);
            return;
        }
		
		f_lseek(&fdst, f_size(&fdst));
        
        if (f_size(&fdst) > 0) {
            const char* newline = "\r\n";
            UINT bytes_written;
            FRESULT res = f_write(&fdst, newline, 2, &bytes_written);
            if (res != FR_OK) {
                printf("Failed to write newline: %d\r\n", res);
            }
        }
	 
		
		My_write_file(Vol_Value);

		printf("%s\r\n",filebuffer);
		OLED_Printf(0,0,16,"%s\r\n",filebuffer);
		OLED_Refresh();
			
		uint32_t actual_length = strlen((char*)filebuffer);
		result = f_write(&fdst, filebuffer, actual_length, &bw);				//��filebuffer�е�����д���ļ��
	
        
		/**********���д���� begin****************/
		if(FR_OK == result)		
                printf("FATFS FILE write Success!\r\n");
        else
		{
                printf("FATFS FILE write failed!\r\n");
        }
		
        f_close(&fdst);//�ر��ļ�
		
	} 
}

void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);	// �����ж����ȼ�����
    nvic_irq_enable(SDIO_IRQn, 0, 0);					// ʹ��SDIO�жϣ����ȼ�Ϊ0
}

void My_write_file(float Vol_Value)
{	
	memset(filebuffer, 0, sizeof(filebuffer));
			
    int len = snprintf((char*)filebuffer, sizeof(filebuffer)-1, 
                      "ADC:%.3fV\r\n",  
                      Vol_Value);
    
 
    if(len < 0) {
        strcpy((char*)filebuffer, "Format error");
    }
        filebuffer[12] = '\0';
}
