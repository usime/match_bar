#include "Function.h"
#include "LED.h"
#include "RTC.h"
#include "Serial.h"
#include "SPI_FLASH.h"
#include "ff.h"
#include "diskio.h"
#include "sdcard.h"
#include "ADC.h"

/************************* 宏定义 *************************/
#define  SFLASH_ID                     0xC84013
#define BUFFER_SIZE                    256
#define TX_BUFFER_SIZE                 BUFFER_SIZE
#define RX_BUFFER_SIZE                 BUFFER_SIZE
#define  FLASH_WRITE_ADDRESS           0x000000
#define  FLASH_READ_ADDRESS            FLASH_WRITE_ADDRESS

/************************ 变量定义 ************************/
uint32_t flash_id = 0;
uint8_t  tx_buffer[TX_BUFFER_SIZE];
uint8_t  rx_buffer[TX_BUFFER_SIZE];
uint16_t i = 0, count, result = 0;
uint8_t  is_successful = 0;
int adc_value;    // ADC采样数字量
float Vol_Value;
float voltage = 3.155;

FIL fdst;
FATFS fs;
UINT br, bw;
//BYTE textfilebuffer[2048] = "GD32MCU FATFS TEST!\r\n";
BYTE buffer[128];
BYTE filebuffer[128];

/************************ 函数定义 ************************/
void nvic_config(void);
void write_file(void);
void My_write_file(void);
void InputSD(void);


void System_Init(void)
{
	systick_config();     // 时钟配置
	ADC_Port_Init();
}
void InputSD(void)
{
	uint16_t k = 5;
	DSTATUS stat = 0;
	do
	{
		stat = disk_initialize(0); 			//初始化SD卡（设备号0）,物理驱动器编号,每个物理驱动器（如硬盘、U 盘等）通常都被分配一个唯一的编号。
	}while((stat != 0) && (--k));			//如果初始化失败，重试最多k次。
    
    printf("SD Card disk_initialize:%d\r\n",stat);
    f_mount(0, &fs);						 //挂载SD卡的文件系统（设备号0）。
    printf("SD Card f_mount:%d\r\n",stat);

	if(RES_OK == stat)						 //返回挂载结果（FR_OK 表示成功）。
	{        
        printf("\r\nSD Card Initialize Success!\r\n");
		
		result = f_open(&fdst, "0:/MyFile.TXT", FA_WRITE | FA_OPEN_ALWAYS);		//在SD卡上创建文件FATFS.TXT。
		
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
		result = f_write(&fdst, filebuffer, actual_length, &bw);				//将filebuffer中的数据写入文件�
	
        
		/**********检查写入结果 begin****************/
		if(FR_OK == result)		
                printf("FATFS FILE write Success!\r\n");
        else
		{
                printf("FATFS FILE write failed!\r\n");
        }
		
        f_close(&fdst);//关闭文件
		
	} 
}

void UsrFunction(void)
{
	nvic_config();		//配置中断控制器
	Led_Init();
	Serial_Init(); //串口初始化
	
	nvic_irq_enable(USART0_IRQn, 0, 0);//使能USART0中断
	
	usart_interrupt_enable(USART0, USART_INT_RBNE);//接收中断打开

	
	while(1)
	{
		gpio_bit_set(GPIOA, GPIO_PIN_5); 
		delay_1ms(500);
		gpio_bit_reset(GPIOA, GPIO_PIN_5);  
		delay_1ms(500);
		
		adc_flag_clear(ADC0,ADC_FLAG_EOC);  				//  去越蕖烁要志
		while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){};  	//  鼙取转刍蕖烁要志
		adc_value = ADC_RDATA(ADC0);    					// 讈取ADC私邼
		Vol_Value = adc_value*3.3/4095;
			
		InputSD();
		
	}
}


void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);	// 设置中断优先级分组
    nvic_irq_enable(SDIO_IRQn, 0, 0);					// 使能SDIO中断，优先级为0
}

/* 通过串口输入写入文件 */
void write_file(void)
{
    printf("Input data (press Enter to save):\r\n");
    
    uint16_t index = 0;    
    while(1){
        if(usart_flag_get(USART0, USART_FLAG_RBNE) != RESET){
            char ch = usart_data_receive(USART0); 						// 如果接收缓冲区非空，从USART0接收一个字符
            if(ch == '\r'){												// 检查接收到的字符是否为回车键（'\r'）
                filebuffer[index] = '\0';  								// 如果是回车键，在当前位置添加字符串结束符 '\0'
                break;													// 跳出循环，结束数据接收
            }
            filebuffer[index++] = ch;        							// 存储接收到的字符    
            if(index >= sizeof(filebuffer)-1) break;					// 如果缓冲区满则结束接收
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
//	systick_config();     // 时钟配置
//	Led_Init();
//	KEY1_Init();
//	KEY2_Init();
//	EXTI_Pin_Init();
//	Serial_Init();
////	gd_eval_com_init();
//	OLED_Init();
//	DMA_ADC_config_Init();
//	RTC_Init();	
//	//按键绑定操作*****************************************
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
//	TIM2_Init(240-1,3000-1);//开启按键的定时器
//}

//void UsrFunction(void)
//{

//	printf("Hello CIMC");
//	//rtc_setup();

//	while(1)
//	{
//		
//		/*LED测试
//		
//		*/
//		
//		/*串口测试，可以接收发送HEX数据包
////		// 在主循环手动设置测试数据
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
//		/*ADC功能测试，可以将ADC值通过串口发送，信息格式可以用Vofa+显示曲线
//		int adc_value;
//		adc_flag_clear(ADC0,ADC_FLAG_EOC);//清除标志位
//		while(SET!=adc_flag_get(ADC0,ADC_FLAG_EOC)){}
//		adc_value=ADC_RDATA(ADC0);			
//		printf("ADC_Value:%d,%.2f\n",adc_value,3.3*(adc_value/4095.0));
//		*/
//		
//		/*ADC功能测试，带数据转运DMA		
//		printf("ADC_Value:%d,%.2f\n",adc_value[0],3.3*(adc_value[0]/4095.0));
//		OLED_Printf(0,16,16,"ADC_V:%d,%.2f\n",adc_value[0],3.3*(adc_value[0]/4095.0));
//		OLED_Refresh(); 
//		*/
//		
//		/*OLED测试，可以使用OLED_Printf()函数进行方便的OLED显示操作
//		OLED_Printf(0, 16, 16, "Ki:%4.2f Ki:%d", 23.33,5);
//		OLED_Refresh(); 
//		*/
//		
//		/* 定时中断测试
//		OLED_Printf(0, 16, 16, "cnt:%5d", cnt);
//		OLED_Refresh(); 
//		*/
//		
//		/*按键逻辑测试
//		OLED_Printf(0, 16, 16, "key_Num:%5d", Key_num);
//		OLED_Refresh(); 
//		*/
//		
//		/*RTC测试
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




