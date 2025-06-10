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
extern struct Button btn5;
extern struct Button btn6;
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
void BTN5_SINGLEClick_Handler(void* btn)
{
	Key_num=5;
}
void BTN6_SINGLEClick_Handler(void* btn)
{
	Key_num=6;
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
	button_init(&btn5, read_button_GPIO, 0, btn5_id);
	button_init(&btn6, read_button_GPIO, 0, btn6_id);
	button_attach(&btn1, SINGLE_CLICK,BTN1_SINGLEClick_Handler);
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
	unsigned char data[]=" ";
	TIM2_Init(99,1999);//1ms一次中断
	 while(1)
	 {
		OLED_ShowString(0,0,"**Hello CIMC**.",16);
		if(cnt-last_update==5){
		last_update=cnt;
		
		}	
	
	 //  获取转换结束标志
		
		sprintf(data,"key_num= %2d V",Key_num);
		OLED_ShowString(0,16,data,16);
	
		OLED_Refresh();
		
	 }
}


/****************************End*****************************/
