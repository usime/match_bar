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
int adc_value;    // ADC����������
float Vol_Value;
float voltage = 3.155;

FIL fdst;
FATFS fs;
UINT br, bw;
//BYTE textfilebuffer[2048] = "GD32MCU FATFS TEST!\r\n";
BYTE buffer[128];
BYTE filebuffer[128];

/************************ �������� ************************/
void nvic_config(void);
void write_file(void);
void My_write_file(void);
void InputSD(void);


void System_Init(void)
{
	systick_config();     // ʱ������
	ADC_Port_Init();
}
void InputSD(void)
{
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
        }else{
			printf("f_open success\r\n");
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
		
		printf("success to write newline\r\n");
	 
		
		My_write_file();
		
		printf("success to write_file\r\n");
		printf("%s\r\n",filebuffer);
			
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

void UsrFunction(void)
{
	nvic_config();		//�����жϿ�����
	Led_Init();
	Serial_Init(); //���ڳ�ʼ��
	
	nvic_irq_enable(USART0_IRQn, 0, 0);//ʹ��USART0�ж�
	
	usart_interrupt_enable(USART0, USART_INT_RBNE);//�����жϴ�

	
	while(1)
	{
		gpio_bit_set(GPIOA, GPIO_PIN_5); 
		delay_1ms(500);
		gpio_bit_reset(GPIOA, GPIO_PIN_5);  
		delay_1ms(500);
		
		adc_flag_clear(ADC0,ADC_FLAG_EOC);  				//  ȥԽޡ˸Ҫ־
		while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){};  	//  ܱȡתۻޡ˸Ҫ־
		adc_value = ADC_RDATA(ADC0);    					// ׁȡADC˽ߝ
		Vol_Value = adc_value*3.3/4095;
			
		InputSD();
		
	}
}


void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);	// �����ж����ȼ�����
    nvic_irq_enable(SDIO_IRQn, 0, 0);					// ʹ��SDIO�жϣ����ȼ�Ϊ0
}

/* ͨ����������д���ļ� */
void write_file(void)
{
    printf("Input data (press Enter to save):\r\n");
    
    uint16_t index = 0;    
    while(1){
        if(usart_flag_get(USART0, USART_FLAG_RBNE) != RESET){
            char ch = usart_data_receive(USART0); 						// ������ջ������ǿգ���USART0����һ���ַ�
            if(ch == '\r'){												// �����յ����ַ��Ƿ�Ϊ�س�����'\r'��
                filebuffer[index] = '\0';  								// ����ǻس������ڵ�ǰλ������ַ��������� '\0'
                break;													// ����ѭ�����������ݽ���
            }
            filebuffer[index++] = ch;        							// �洢���յ����ַ�    
            if(index >= sizeof(filebuffer)-1) break;					// ��������������������
        }
    }
}

void My_write_file(void)
{	
	memset(filebuffer, 0, sizeof(filebuffer));
			
    int len = snprintf((char*)filebuffer, sizeof(filebuffer)-1, 
                      "ADC:%.3fV\r\n",  
                      voltage);
    
 
    if(len < 0) {
        strcpy((char*)filebuffer, "Format error");
    }
        filebuffer[12] = '\0';

}





















//#include "HeaderFiles.h"
//#include "Function.h"
//int FLAG=0;
uint8_t Key_num=0;
//void BTN1_SINGLEClick_Handler(void* btn)
//{
//	Key_num=1;
//}
//void BTN1_LONG_PRESS_HOLD_Handler(void* btn)
//{
//	Key_num=10;
//}
//void BTN2_SINGLEClick_Handler(void* btn)
//{
//	Key_num=2;
//}
//void BTN3_SINGLEClick_Handler(void* btn)
//{
//	Key_num=3;
//}
//void BTN4_SINGLEClick_Handler(void* btn)
//{
//	Key_num=4;
//}
//void BTN5_SINGLEClick_Handler(void* btn)
//{
//	Key_num=5;
//}
//void BTN6_SINGLEClick_Handler(void* btn)
//{
//	Key_num=6;
//}

//void System_Init(void)
//{
//	systick_config();     // ʱ������
//	Led_Init();
//	KEY1_Init();
//	KEY2_Init();
//	EXTI_Pin_Init();
//	Serial_Init();
////	gd_eval_com_init();
//	OLED_Init();
//	DMA_ADC_config_Init();
//	RTC_Init();	
//	//�����󶨲���*****************************************
//	button_init(&btn1, read_button_GPIO, 0, btn1_id);
//	button_init(&btn2, read_button_GPIO, 0, btn2_id);
//	button_init(&btn3, read_button_GPIO, 0, btn3_id);
//	button_init(&btn4, read_button_GPIO, 0, btn4_id);
//	button_init(&btn5, read_button_GPIO, 0, btn5_id);
//	button_init(&btn6, read_button_GPIO, 0, btn6_id);
//	button_attach(&btn1, SINGLE_CLICK,BTN1_SINGLEClick_Handler);
//	button_attach(&btn1, LONG_PRESS_HOLD ,BTN1_LONG_PRESS_HOLD_Handler);
//	button_attach(&btn2, SINGLE_CLICK,BTN2_SINGLEClick_Handler);
//	button_attach(&btn3, SINGLE_CLICK,BTN3_SINGLEClick_Handler);
//	button_attach(&btn4, SINGLE_CLICK,BTN4_SINGLEClick_Handler);
//	button_attach(&btn5, SINGLE_CLICK,BTN5_SINGLEClick_Handler);
//	button_attach(&btn6, SINGLE_CLICK,BTN6_SINGLEClick_Handler);
//	button_start(&btn1);
//	button_start(&btn2);
//	button_start(&btn3);
//	button_start(&btn4);
//	button_start(&btn5);
//	button_start(&btn6);
//	//**************************************************
//	TIM2_Init(240-1,3000-1);//���������Ķ�ʱ��
//}

//void UsrFunction(void)
//{

//	printf("Hello CIMC");
//	//rtc_setup();

//	while(1)
//	{
//		
//		/*LED����
//		
//		*/
//		
//		/*���ڲ��ԣ����Խ��շ���HEX���ݰ�
////		// ����ѭ���ֶ����ò�������
////		Serial_RxPacket[0] = 0xAA; 
////		Serial_RxFlag = 1;
//		OLED_ShowNum(1,1,5,2,16);
//		OLED_Refresh();
//		if (Serial_GetRxFlag() == 1)
//		{
//			//value = atoi((char*)Serial_RxPacket);
//			//OLED_ShowNum(2,1,value,4);
//			OLED_ShowHexNum(10,15, Serial_RxPacket[0], 2);
//			OLED_ShowHexNum(20,15, Serial_RxPacket[1], 2);
//			OLED_ShowHexNum(30,15, Serial_RxPacket[2], 2);
//			OLED_ShowHexNum(40,15, Serial_RxPacket[3], 2);
//		}
//		*/
//		
//		/*ADC���ܲ��ԣ����Խ�ADCֵͨ�����ڷ��ͣ���Ϣ��ʽ������Vofa+��ʾ����
//		int adc_value;
//		adc_flag_clear(ADC0,ADC_FLAG_EOC);//�����־λ
//		while(SET!=adc_flag_get(ADC0,ADC_FLAG_EOC)){}
//		adc_value=ADC_RDATA(ADC0);			
//		printf("ADC_Value:%d,%.2f\n",adc_value,3.3*(adc_value/4095.0));
//		*/
//		
//		/*ADC���ܲ��ԣ�������ת��DMA		
//		printf("ADC_Value:%d,%.2f\n",adc_value[0],3.3*(adc_value[0]/4095.0));
//		OLED_Printf(0,16,16,"ADC_V:%d,%.2f\n",adc_value[0],3.3*(adc_value[0]/4095.0));
//		OLED_Refresh(); 
//		*/
//		
//		/*OLED���ԣ�����ʹ��OLED_Printf()�������з����OLED��ʾ����
//		OLED_Printf(0, 16, 16, "Ki:%4.2f Ki:%d", 23.33,5);
//		OLED_Refresh(); 
//		*/
//		
//		/* ��ʱ�жϲ���
//		OLED_Printf(0, 16, 16, "cnt:%5d", cnt);
//		OLED_Refresh(); 
//		*/
//		
//		/*�����߼�����
//		OLED_Printf(0, 16, 16, "key_Num:%5d", Key_num);
//		OLED_Refresh(); 
//		*/
//		
//		/*RTC����
//		rtc_show_time();
//		delay_1ms(1000);
//		*/
//		rtc_show_time();
////		delay_1ms(1000);		
//		
//		//delay_1ms(1000);
//		if(Key_num==4){	
//		FLAG=1;
//		Key_num=0;
////		rtc_setup();						
//		OLED_Clear();
//		current_state = ADJ_YEAR;
//		while(current_state <ADJ_CONFIRM){
//		handle_key_event(Key_num);
//		show_adjust_status();
//		}
//		apply_rtc_changes();
//		OLED_Clear();
//		}

////		for(int i=0;i<70;i++){
////		OLED_Printf(0,0,12,"  %0.2x\r\n",dec2bcd(i));
////		OLED_Refresh();
////		}
////		delay_1ms(300);
//	}
//}




