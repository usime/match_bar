#include "stm32f10x.h"                  // Device header
#include "Motor.h"
/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
#define PWM_MAX 3070
//枚举变量，存三种控制模式
int Motor_en_flag;
 Motor_Ctrl_choose Current_State;
extern int Flag_Motor_en;
const char* get_state_name(Current_State) {
    switch(Current_State) {
        case Forward:    return "Forward" ;
        case Back: return "Back";
        case Stop:   return "Stop";
      
    }
}
void Motor_En_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						//将PA4和PA5引脚初始化为推挽输出												//初始化直流电机的底层PWM
}

void Motor_Init_Right(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOA的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PA4和PA5引脚初始化为推挽输出											//初始化直流电机的底层PWM
}
//左电机控制函数 可以有三种选择模式 前进 后退 停止
void Motor_Left_Control(Motor_Ctrl_choose Options){
if(Options==Forward){
GPIO_SetBits(GPIOA,GPIO_Pin_11);
GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}
else if(Options==Back){
GPIO_ResetBits(GPIOA,GPIO_Pin_11);
GPIO_SetBits(GPIOA,GPIO_Pin_12);
}
else if(Options==Stop){
GPIO_ResetBits(GPIOA,GPIO_Pin_11);
GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}
}
//右电机控制函数 可以有三种选择模式 前进 后退 停止
void Motor_Right_Control(Motor_Ctrl_choose Options){
if(Options==Forward){
GPIO_SetBits(GPIOB,GPIO_Pin_14);
GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}
else if(Options==Back){
GPIO_ResetBits(GPIOB,GPIO_Pin_14);
GPIO_SetBits(GPIOB,GPIO_Pin_15);
}
else if(Options==Stop){
GPIO_ResetBits(GPIOB,GPIO_Pin_14);
GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}
}
//左右电机的PWM装载、绝对值函数和限幅函数
void Load_Motor_pwm(int Motor1_pwm,int Motor2_pwm){
if(Motor1_pwm>0){
Motor1_pwm=(Motor1_pwm>0)?Motor1_pwm:(-Motor1_pwm);//正负判断
Motor2_pwm=(Motor2_pwm>0)?Motor2_pwm:(-Motor2_pwm);//正负判断
Motor1_pwm=(Motor1_pwm>=PWM_MAX)?PWM_MAX:Motor1_pwm;//进行百分之60占空比积分限幅
Motor2_pwm=(Motor2_pwm>=PWM_MAX)?PWM_MAX:Motor2_pwm;//进行百分之60占空比积分限幅
TIM_SetCompare4(TIM3, Motor1_pwm);	//左轮定时器4通道1
TIM_SetCompare1(TIM3, Motor2_pwm);	//右轮定时器4通道2
Motor_Ctrl_choose(Forward);
}
else{
Motor1_pwm=(Motor1_pwm>0)?Motor1_pwm:(-Motor1_pwm);//正负判断
Motor2_pwm=(Motor2_pwm>0)?Motor2_pwm:(-Motor2_pwm);//正负判断
Motor1_pwm=(Motor1_pwm>=PWM_MAX)?PWM_MAX:Motor1_pwm;//进行百分之60占空比积分限幅
Motor2_pwm=(Motor2_pwm>=PWM_MAX)?PWM_MAX:Motor2_pwm;//进行百分之60占空比积分限幅
TIM_SetCompare4(TIM3, Motor1_pwm);	//左轮定时器4通道1
TIM_SetCompare1(TIM3, Motor2_pwm);	//右轮定时器4通道2
Motor_Ctrl_choose(Back);
}
}



//直接使能和失能函数可以有更好的刹车启动效果
void Set_Motor_Enable(){
GPIO_SetBits(GPIOA,GPIO_Pin_12);
Motor_en_flag=1;
}

void Set_Motor_Disable(){
GPIO_ResetBits(GPIOA,GPIO_Pin_12);
Motor_en_flag=0;
}

/**
  * 函    数：直流电机设置速度
  * 参    数：Speed 要设置的速度，范围：-100~100
  * 返 回 值：无
  */




