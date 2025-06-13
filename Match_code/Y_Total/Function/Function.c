#include "Function.h"   

int Adc_value;  
float Vol_Value;

//按键的两个变量定义
int FLAG=0;
uint8_t Key_num=0;

void BTN1_SINGLEClick_Handler(void* btn)
{
	Key_num=1;
}
void BTN1_LONG_PRESS_HOLD_Handler(void* btn)
{
	Key_num=10;
}
void BTN2_SINGLEClick_Handler(void* btn)
{
	Key_num=2;
}
void BTN3_SINGLEClick_Handler(void* btn)
{
	Key_num=3;
}
void BTN4_SINGLEClick_Handler(void* btn)
{
	Key_num=4;
}
void BTN5_SINGLEClick_Handler(void* btn)
{
	Key_num=5;
}
void BTN6_SINGLEClick_Handler(void* btn)
{
	Key_num=6;
}

void System_Init(void)
{
	systick_config();     // 时钟配置
	ADC_Port_Init();
	Led_Init();
//	gd_eval_com_init();

	Led_Init();//正常亮灭，闪烁
	KEY1_Init();
	KEY2_Init();
	EXTI_Pin_Init();
	Serial_Init();
	OLED_Init();
//	ADC_Init();
//	DMA_ADC_config_Init();
//	RTC_Init();
	
	//********************************
	button_init(&btn1, read_button_GPIO, 0, btn1_id);
	button_init(&btn2, read_button_GPIO, 0, btn2_id);
	button_init(&btn3, read_button_GPIO, 0, btn3_id);
	button_init(&btn4, read_button_GPIO, 0, btn4_id);
	button_init(&btn5, read_button_GPIO, 0, btn5_id);
	button_init(&btn6, read_button_GPIO, 0, btn6_id);
	button_attach(&btn1, SINGLE_CLICK,BTN1_SINGLEClick_Handler);
	button_attach(&btn1, LONG_PRESS_HOLD ,BTN1_LONG_PRESS_HOLD_Handler);
	button_attach(&btn2, SINGLE_CLICK,BTN2_SINGLEClick_Handler);
	button_attach(&btn3, SINGLE_CLICK,BTN3_SINGLEClick_Handler);
	button_attach(&btn4, SINGLE_CLICK,BTN4_SINGLEClick_Handler);
	button_attach(&btn5, SINGLE_CLICK,BTN5_SINGLEClick_Handler);
	button_attach(&btn6, SINGLE_CLICK,BTN6_SINGLEClick_Handler);
	button_start(&btn1);
	button_start(&btn2);
	button_start(&btn3);
	button_start(&btn4);
	button_start(&btn5);
	button_start(&btn6);
	//*************************************
	TIM2_Init(240-1,3000-1);//开启按键的定时器
	
}

void UsrFunction(void)
{	
	printf("UsrFunction\r\n");
	while(1)
	{
		gpio_bit_set(GPIOA, GPIO_PIN_5); 
		delay_1ms(500);
		gpio_bit_reset(GPIOA, GPIO_PIN_5);  
		delay_1ms(500);
		
		adc_flag_clear(ADC0,ADC_FLAG_EOC);  				
		while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){}  	
		Adc_value = ADC_RDATA(ADC0);    					
		Vol_Value = Adc_value*3.3/4095;
			
		InputSD(Vol_Value);
		
	}
}
