/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：main.c
 * 作者: Lingyu Meng
 * 平台: 2025 CIMC IHD V04
 * 版本: Lingyu Meng     2023/2/16     V0.01    original
************************************************************/

/************************* 头文件 *************************/
int Key_num=0;
extern int cnt;
#include "HeaderFiles.h"
extern struct Button btn1;
extern struct Button btn2;
extern struct Button btn3;
extern struct Button btn4;
void BTN1_SINGLEClick_Handler(void* btn)
{
	Key_num=1;
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
/************************ 执行函数 ************************/

int main(void)
{
	int last_update=0;
	System_Init();   // 系统初始化
	button_init(&btn1, read_button_GPIO, 0, btn1_id);
	button_init(&btn2, read_button_GPIO, 0, btn2_id);
	button_init(&btn3, read_button_GPIO, 0, btn3_id);
	button_init(&btn4, read_button_GPIO, 0, btn4_id);
	button_attach(&btn1, SINGLE_CLICK,BTN1_SINGLEClick_Handler);
	button_attach(&btn2, SINGLE_CLICK,BTN2_SINGLEClick_Handler);
	button_attach(&btn3, SINGLE_CLICK,BTN3_SINGLEClick_Handler);
	button_attach(&btn4, SINGLE_CLICK,BTN4_SINGLEClick_Handler);
	button_start(&btn1);
	button_start(&btn2);
	button_start(&btn3);
	button_start(&btn4);
	 unsigned char data[]=" ";
	delay_1ms(10);
	 while(1)
	 {
		OLED_ShowString(0,0,"**Hello CIMC**.",16);
		if(cnt-last_update==50){
		last_update=cnt;
		button_ticks();
		}	
	
	 //  获取转换结束标志
		
		sprintf(data,"ADC= %.2d V",Key_num);
		OLED_ShowString(0,16,data,16);
	
		OLED_Refresh();
		
	 }
}


/****************************End*****************************/
