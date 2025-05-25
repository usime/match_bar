#include "stm32f10x.h"                  // Device header
#include "multibutton.h"
#include "Key.h"
//可以扩展按键，增加宏定义即可
#define Key1_pin GPIO_Pin_4
#define Key2_pin GPIO_Pin_5
#define Key3_pin GPIO_Pin_15

//Button结构体名称是multibutton里面带的，不能改，只改结构体变量名称
struct Button btn1;
struct Button btn2;
struct Button btn3;
void Key_Init(void)
{
 //按键基础配置初始化
}

//返回按键电平函数，必须具备的，这个可以在switch-case里面扩展按键选项，不用改。
uint8_t read_button_GPIO(uint8_t button_id)
{
	switch(button_id)
	{
		case btn1_id:
			return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
		case btn2_id:
			return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
		case btn3_id:
			return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15);
		default:
			return 0;
	}
}
//下面为按键要执行对应动作的回调函数，即每次符合对应的按键的动作时都回执行下面的对应按键的回调函数
void BTN1_SINGLE_Click_Handler(void)
{
	//do something
}

void BTN2_SINGLE_Click_Handler(void)
{
	//do something
}
void BTN3_SINGLE_Click_Handler(void)
{
	//do something
}
