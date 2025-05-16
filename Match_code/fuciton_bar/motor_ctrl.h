#ifndef __MOTOR_CTRL_H__
#define __MOTOR_CTRL_H__

#include "main.h"

/*-------------------------------------------------------------------------------------------*/
/*---------------------------------------PID����---------------------------------------------*/
/*-------------------------------------------------------------------------------------------*/
/*PID�ṹ�岿��*/
/*PID����Ȩ��*/
/*Ŀ��ֵ����ȡֵ���������*/
/*���������֡�΢������������*/
/*����޷��Լ������޷�*/	
typedef struct 
{
  float Kp;					//P����
  float Ki;					//I����
  float Kd;					//D����
	float Error_Internal;	
  float Target;				//Ŀ��ֵ
  float Actual;			//��ǰ��ȡֵ
  float Error;		//�������
	float Error_Last;		//�������
  float Error_Last_Last;		//�������	
  float KpOut;				//�������
  float KiOut;				//�������
  float KdOut;				//΢�����
  float PID_Out;			//PID�����
	float difference;
  uint32_t PID_Limit_MAX; 	//�������޷�
  uint32_t Ki_Limit_MAX;	//����������޷�
}PID_InitTyPedef;
float PID_Calculate(PID_InitTyPedef *pid,float Actual,float Target);
void Set_PID_Param(PID_InitTyPedef*pid, float P, float I,float D);
void Set_Target_val(PID_InitTyPedef *pid,float Target_val);
void Set_Motor_Disable(void);
void Set_Motor_Enable (void);
/*-------------------------------------------------------------------------------------------*/
/*------------------------------------�����������-------------------------------------------*/
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
//PID����
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
