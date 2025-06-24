#ifndef __MOTOR_H
#define __MOTOR_H
typedef enum{
Forward,
Back,
Stop,
}Motor_Ctrl_choose;

extern Motor_Ctrl_choose Current_State;


void Motor_En_Init(void);
void Motor_Init_Right(void);
void Motor_Left_Control(Motor_Ctrl_choose Options);
void Motor_Right_Control(Motor_Ctrl_choose Options);
void Load_Motor_pwm(int Motor1_pwm,int Motor2_pwm);
void Motor_SetSpeed(int Speed);
//直接操作左右轮子的使能口
void Set_Motor_Enable(void);
void Set_Motor_Disable(void);
#endif


