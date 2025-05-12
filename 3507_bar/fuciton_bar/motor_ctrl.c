#include "motor_ctrl.h"

uint8_t MOTOR1_ENABLE_FLAG = 1;//电机使能标志位
uint8_t MOTOR2_ENABLE_FLAG = 1;


uint8_t Turn_PID_Flag = 0;
uint8_t Distance_PID_Flag = 0;
uint8_t Gyro_PID_Flag = 0;
uint8_t Angle_PID_Flag = 0;
extern float Angle;

/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------PID计算部分部分---------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
//速度环参数修改
PID_InitTyPedef pid_Motor1_Speed ={	\
	.Kp = 0,				\
	.Ki = 0,				\
	.Kd = 0,				\
	.Target = 0,			\
	.Actual = 0,			\
	.Error=0,				\
	.Error_Internal=0,\
	.Error_Last=0,  \
	.Error_Last_Last=0,\
	.KpOut = 0,				\
	.KiOut = 0,				\
	.KdOut = 0,				\
	.PID_Out = 0,			\
	.PID_Limit_MAX = 9999,	\
	.Ki_Limit_MAX = 9999,	\
	.difference=0,\
};
PID_InitTyPedef pid_Motor2_Speed ={	\
	.Kp = 5,				\
	.Ki = 0,				\
	.Kd = 0,				\
	.Target = 0,			\
	.Actual = 0,			\
	.Error=0,				\
	.Error_Last=0,  \
	.Error_Last_Last=0,		\
	.KpOut = 0,				\
	.KiOut = 0,				\
	.KdOut = 0,				\
	.PID_Out = 0,			\
	.Error_Internal=0,\
	.PID_Limit_MAX = 9999,	\
	.Ki_Limit_MAX = 9999,	\
	.difference=0,\
};
//转向环参数修改			60cm/s	6	140
PID_InitTyPedef pid_Turn ={			\
	.Kp = 0,				  \
	.Ki = 0,				  \
	.Kd = 0,				  \
	.Target = 0,			\
	.Actual = 0,			\
	.Error=0,				  \
	.Error_Last=0,    \
	.Error_Last_Last=0,			\
	.KpOut = 0,				\
	.KiOut = 0,				\
	.Error_Internal=0,\
	.KdOut = 0,				\
	.PID_Out = 0,			\
	.PID_Limit_MAX = 9999,	\
	.Ki_Limit_MAX = 9999,	\
	.difference=0,\
};
//距离环参数修改
PID_InitTyPedef pid_Distance ={		\
	.Kp = 10,				\
	.Ki = 0,				\
	.Kd = 5,				\
	.Target = 0,			\
	.Actual = 0,			\
	.Error=0,				\
	.Error_Last=0,  \
	.Error_Last_Last=0,\
	.KpOut = 0,				\
	.KiOut = 0,				\
	.Error_Internal=0,\
	.KdOut = 0,				\
	.PID_Out = 0,			\
	.PID_Limit_MAX = 9999,	\
	.Ki_Limit_MAX = 9999,	\
	.difference=0,\
};
//角速度环参数修改
PID_InitTyPedef pid_Gyro ={			\
	.Kp = 0.09,				\
	.Ki = 0.015,				\
	.Kd = 0,				\
	.Target = 0,			\
	.Actual = 0,			\
  .Error=0,				\
	.Error_Internal=0,\
	.Error_Last=0,  \
	.Error_Last_Last=0,\
	.KpOut = 0,				\
	.KiOut = 0,				\
	.KdOut = 0,				\
	.PID_Out = 0,			\
	.PID_Limit_MAX = 9999,	\
	.Ki_Limit_MAX = 9999,	\
};
//转向环参数修改
PID_InitTyPedef pid_Angle ={			\
	.Kp = 5,				\
	.Ki = 0,				\
	.Kd = 30,				\
	.Target = 0,			\
	.Actual = 0,			\
	.Error=0,				\
	.Error_Internal=0,\
	.Error_Last=0,  \
	.Error_Last_Last=0,\
	.KpOut = 0,				\
	.KiOut = 0,				\
	.KdOut = 0,				\
	.PID_Out = 0,			\
	.PID_Limit_MAX = 1000,	\
	.Ki_Limit_MAX = 1000,	\
};


PID_InitTyPedef trace_hd ={			\
	.Kp = 5,				\
	.Ki = 0,				\
	.Kd = 30,				\
	.Target = 0,			\
	.Actual = 0,			\
	.Error=0,				\
	.Error_Internal=0,\
	.Error_Last=0,  \
	.Error_Last_Last=0,\
	.KpOut = 0,				\
	.KiOut = 0,				\
	.KdOut = 0,				\
	.PID_Out = 0,			\
	.PID_Limit_MAX = 1000,	\
	.Ki_Limit_MAX = 1000,	\
};
//PWM限幅函数 *a传入要限幅的参数  ABS_MAX限幅大小
void PID_Limit(float *a, float ABS_MAX){
  if (*a > ABS_MAX)
    *a = ABS_MAX;
  if (*a < -ABS_MAX)
    *a = -ABS_MAX;
}
/*************************************************************************************************
*	函 数 名:	PID_Calculate
*
*	函数功能:	PID计算公式
*
*   参    数：  PID结构体，当前值，目标值
*************************************************************************************************/
float PID_Calculate(PID_InitTyPedef *pid,float Actual,float Target)
{
	pid->Error = Target - Actual;//计算当前误差 
  pid->Error_Internal+=pid->Error;
	 pid->Error_Internal=( pid->Error_Internal>10000)? pid->Error_Internal=10000:(( pid->Error_Internal<-10000)? pid->Error_Internal=-10000: pid->Error_Internal);
  pid->Actual=pid->Kp*pid->Error+pid->Ki*pid->Error_Internal+pid->Kd*pid->Error;
	pid->Error_Last = pid->Error;
    return pid->Actual;
}
void Set_PID_Param(PID_InitTyPedef *pid, float P, float I,float D)
{
	pid->Kp = P;pid->Ki = I;pid->Kd = D;
	pid->KpOut = 0;pid->KiOut = 0;pid->KdOut = 0;pid->PID_Out = 0;//输出清零
}



//PWM限幅函数 *a传入要限幅的参数  ABS_MAX限幅大小
void PWM_Limit(int *a, int ABS_MAX){
  if (*a > ABS_MAX)
    *a = ABS_MAX;
  if (*a < -ABS_MAX)
    *a = -ABS_MAX;
}
// 设置电机1的PWM
void Set_Motor1_PWM(int Target_PWM){
	PWM_Limit(&Target_PWM,9999);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST,Target_PWM,GPIO_PWM_0_C1_IDX);
}
// 设置电机2的PWM
void Set_Motor2_PWM(int Target_PWM){
	PWM_Limit(&Target_PWM,9999);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST,Target_PWM,GPIO_PWM_0_C0_IDX);
}
//设置电机1的速度
void Set_Motor1_Speed(int Target_Speed){
	if(MOTOR1_ENABLE_FLAG==1)
	{if(Target_Speed >= 0)//正转
	{Set_Motor1_PWM(Target_Speed);Motor1_Forward();}
	else if(Target_Speed < 0)//反转
	{Set_Motor1_PWM(-Target_Speed);Motor1_Backward();}}
	else {Motor1_Stop();pid_Motor1_Speed.KpOut = 0;pid_Motor1_Speed.KiOut = 0;pid_Motor1_Speed.KdOut = 0;pid_Motor1_Speed.PID_Out = 0;}//输出清零
}
//设置电机2的速度
void Set_Motor2_Speed(int Target_Speed){
	if(MOTOR2_ENABLE_FLAG==1)
	{if(Target_Speed >= 0)//正转
	{Set_Motor2_PWM(Target_Speed);Motor2_Forward();}
	else if(Target_Speed < 0)//反转
	{Set_Motor2_PWM(-Target_Speed);Motor2_Backward();}}
	else {Motor2_Stop();pid_Motor2_Speed.KpOut = 0;pid_Motor2_Speed.KiOut = 0;pid_Motor2_Speed.KdOut = 0;pid_Motor2_Speed.PID_Out = 0;}//输出清零
}
//设置所有电机速度
void SET_MOTORS_SPEED(int Target_Motor1_Speed,int Target_Motor2_Speed){
	Set_Motor1_Speed(Target_Motor1_Speed);Set_Motor2_Speed(Target_Motor2_Speed);
}

void Set_Motor_Disable(){
DL_TimerA_disableClock(PWM_0_INST);
}

void Set_Motor_Enable(){
DL_TimerA_enableClock(PWM_0_INST);
}
