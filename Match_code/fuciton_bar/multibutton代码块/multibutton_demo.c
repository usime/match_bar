#include "stm32f10x.h"               
#include "Delay.h"
#include "OLED.h"
#include "multibutton.h"
#include "Key.h" 			
extern Button btn1;
extern Button btn2;
extern Button btn3;
int main(void)
{
	OLED_Init();		
    //对应按键初始化									
	Key_3_Init();
	Key_1_2_Init();
    //需要先进行button按键事情绑定初始化
	button_init(&btn1, read_button_GPIO, 0, btn1_id);
	button_init(&btn2, read_button_GPIO, 0, btn2_id);
	button_init(&btn3, read_button_GPIO, 0, btn3_id);
    //进行button按键事情绑定，
    /*
    @parm1:按键结构体指针变量
    @parm2:按键执行动作
    @parm3:符合按键执行动作后自动调用的回调函数
    */
	button_attach(&btn1, DOUBLE_CLICK,BTN1_SINGLE_Click_Handler);
	button_attach(&btn2, DOUBLE_CLICK,BTN2_SINGLE_Click_Handler);
	button_attach(&btn3, DOUBLE_CLICK,BTN3_SINGLE_Click_Handler);
    /*
    按键开始
    */
	button_start(&btn1);
	button_start(&btn2);
	button_start(&btn3);
	while (1)
{
    //button_ticks()要每5ms执行一次，这里简单模拟了一下
	Delay_ms(5);
	button_ticks();
}
	return 0;
}

 
 
 
 
		

