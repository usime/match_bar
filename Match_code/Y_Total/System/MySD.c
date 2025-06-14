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

/************************* ºê¶¨Òå *************************/
#define  SFLASH_ID                     0xC84013
#define BUFFER_SIZE                    256
#define TX_BUFFER_SIZE                 BUFFER_SIZE
#define RX_BUFFER_SIZE                 BUFFER_SIZE
#define  FLASH_WRITE_ADDRESS           0x000000
#define  FLASH_READ_ADDRESS            FLASH_WRITE_ADDRESS

/************************ ±äÁ¿¶¨Òå ************************/
uint32_t flash_id = 0;
uint8_t  tx_buffer[TX_BUFFER_SIZE];
uint8_t  rx_buffer[TX_BUFFER_SIZE];
uint16_t i = 0, count, result = 0;
uint8_t  is_successful = 0;
extern int Adc_value;    // ADC²ÉÑùÊý×ÖÁ¿
extern float Vol_Value;

FIL fdst;
FATFS fs;
UINT br, bw;
BYTE buffer[128];
BYTE filebuffer[128];


void InputSD(float Vol_Value)
{
	nvic_config();		//ÅäÖÃÖÐ¶Ï¿ØÖÆÆ÷
	uint16_t k = 5;
	DSTATUS stat = 0;

	do
	{
		stat = disk_initialize(0); 			//³õÊ¼»¯SD¿¨£¨Éè±¸ºÅ0£©,ÎïÀíÇý¶¯Æ÷±àºÅ,Ã¿¸öÎïÀíÇý¶¯Æ÷£¨ÈçÓ²ÅÌ¡¢U ÅÌµÈ£©Í¨³£¶¼±»·ÖÅäÒ»¸öÎ¨Ò»µÄ±àºÅ¡£
	}while((stat != 0) && (--k));			//Èç¹û³õÊ¼»¯Ê§°Ü£¬ÖØÊÔ×î¶àk´Î¡£
    
    printf("SD Card disk_initialize:%d\r\n",stat);
    f_mount(0, &fs);						 //¹ÒÔØSD¿¨µÄÎÄ¼þÏµÍ³£¨Éè±¸ºÅ0£©¡£
    printf("SD Card f_mount:%d\r\n",stat);

	if(RES_OK == stat)						 //·µ»Ø¹ÒÔØ½á¹û£¨FR_OK ±íÊ¾³É¹¦£©¡£
	{        
        printf("\r\nSD Card Initialize Success!\r\n");
		
		result = f_open(&fdst, "0:/MyFile.TXT", FA_WRITE | FA_OPEN_ALWAYS);		//ÔÚSD¿¨ÉÏ´´½¨ÎÄ¼þFATFS.TXT¡£
		
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
		result = f_write(&fdst, filebuffer, actual_length, &bw);				//½«filebufferÖÐµÄÊý¾ÝÐ´ÈëÎÄ¼þ¡
	
        
		/**********¼ì²éÐ´Èë½á¹û begin****************/
		if(FR_OK == result)		
                printf("FATFS FILE write Success!\r\n");
        else
		{
                printf("FATFS FILE write failed!\r\n");
        }
		
        f_close(&fdst);//¹Ø±ÕÎÄ¼þ
		
	} 
}

void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);	// ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶·Ö×é
    nvic_irq_enable(SDIO_IRQn, 0, 0);					// Ê¹ÄÜSDIOÖÐ¶Ï£¬ÓÅÏÈ¼¶Îª0
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
