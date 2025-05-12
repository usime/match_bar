#ifndef __MOTOR_CTRL_H__
#define __MOTOR_CTRL_H__

#include "main.h"

/*-------------------------------------------------------------------------------------------*/
/*---------------------------------------PID函数---------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
/*PID结构体部分*/
/*PID三个权重*/
/*目标值、获取值、三个误差*/
/*比例、积分、微分输出、总输出*/
/*输出限幅以及积分限幅*/	
typedef struct 
{
  float Kp;					//P参数
  float Ki;					//I参数
  float Kd;					//D参数
	float Error_Internal;	
  float Target;				//目标值
  float Actual;			//当前获取值
  float Error;		//三次误差
	float Error_Last;		//三次误差
  float Error_Last_Last;		//三次误差	
  float KpOut;				//比例输出
  float KiOut;				//积分输出
  float KdOut;				//微分输出
  float PID_Out;			//PID总输出
	float difference;
  uint32_t PID_Limit_MAX; 	//最大输出限幅
  uint32_t Ki_Limit_MAX;	//最大积分输出限幅
}PID_InitTyPedef;
float PID_Calculate(PID_InitTyPedef *pid,float Actual,float Target);
void Set_PID_Param(PID_InitTyPedef*pid, float P, float I,float D);
void Set_Target_val(PID_InitTyPedef *pid,float Target_val);
void Set_Motor_Disable(void);
void Set_Motor_Enable (void);
/*-------------------------------------------------------------------------------------------*/
/*------------------------------------电机操作函数-------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
#define MOTORS_ENABLE()			{MOTOR1_ENABLE_FLAG = 1;MOTOR2_ENABLE_FLAG = 1;}
#define MOTORS_DISABLE()		{MOTOR1_ENABLE_FLAG = 0;MOTOR2_ENABLE_FLAG = 0;}

#define AIN1(x)   x?DL_GPIO_setPins(Motor_Ctrl_AIN1_PORT, Motor_Ctrl_AIN1_PIN):DL_GPIO_clearPins(Motor_Ctrl_AIN1_PORT, Motor_Ctrl_AIN1_PIN)
#define AIN2(x)   x?DL_GPIO_setPins(Motor_Ctrl_AIN2_PORT, Motor_Ctrl_AIN2_PIN):DL_GPIO_clearPins(Motor_Ctrl_AIN2_PORT, Motor_Ctrl_AIN2_PIN)
#define BIN1(x)   x?DL_GPIO_setPins(Motor_Ctrl_BIN1_PORT, Motor_Ctrl_BIN1_PIN):DL_GPIO_clearPins(Motor_Ctrl_BIN1_PORT, Motor_Ctrl_BIN1_PIN)
#define BIN2(x)   x?DL_GPIO_setPins(Motor_Ctrl_BIN2_PORT, Motor_Ctrl_BIN2_PIN):DL_GPIO_clearPins(Motor_Ctrl_BIN2_PORT, Motor_Ctrl_BIN2_PIN)

#define Motor1_Forward()	{AIN1(1);AIN2(0);}
#define Motor1_Backward()	{AIN1(0);AIN2(1);}
#define Motor1_Stop()		{AIN1(0);AIN2(0);}

#define Motor2_Forward()	{BIN1(1);BIN2(0);}
#define Motor2_Backward()	{BIN1(0);BIN2(1);}
#define Motor2_Stop()		{BIN1(0);BIN2(0);}

void Set_Motor1_Speed(int Target_Speed);
void Set_Motor2_Speed(int Target_Speed);
void SET_MOTORS_SPEED(int Target_Motor1_Speed,int Target_Motor2_Speed);
void Set_Motor2_PWM(int Target_PWM);
void Set_Motor1_PWM(int Target_PWM);

extern uint8_t MOTOR1_ENABLE_FLAG;
extern uint8_t MOTOR2_ENABLE_FLAG;
//PID参数
extern PID_InitTyPedef pid_Motor1_Speed;
extern PID_InitTyPedef pid_Motor2_Speed;
extern PID_InitTyPedef pid_Turn;
extern PID_InitTyPedef pid_Distance;
extern PID_InitTyPedef pid_Gyro;
extern PID_InitTyPedef pid_Angle;
extern PID_InitTyPedef trace_hd;
extern uint8_t Turn_PID_Flag;
extern uint8_t Distance_PID_Flag;
extern uint8_t Gyro_PID_Flag;
extern uint8_t Angle_PID_Flag;



#endif
