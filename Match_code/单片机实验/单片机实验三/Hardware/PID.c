#include "stm32f10x.h"                  // Device header
#include "PID.h"
long Interal_Motorpulse1;
long Interal_Motorpulse2;
short  Unit_Interal_Motorpulse1;
short  Unit_Interal_Motorpulse2;
PIDTypdDef Motor_Left;
PIDTypdDef Motor_Right;
float Motor1_pwm;
float Motor2_pwm;
float kp_k210=0,kd_k210=0;//先调KP，当出现高频震荡后开始调KD
float actualvalue,expectvalue;
float error_k210,error_k210_last;
float compensate_pwm=0;//补偿PWM
extern PIDTypdDef*PID_Location;
extern PIDTypdDef*PID_Speed;
int mypid[3]={0,0,0};
float expect_dist;
PIDTypdDef Motor_Ctrl;
//速度环PID参数初始化
void PID_Speed_Init(PIDTypdDef*PID_Speed){
PID_Speed->err_last=0;
PID_Speed->err_now=0;
PID_Speed->Kd=0;
PID_Speed->Ki=0;
PID_Speed->Kp=0;
PID_Speed->target_val=0;
PID_Speed->actual_val=0;
PID_Speed->Unit_Interal_Motorpulse1=0;//左轮单位时间捕获到的脉冲数
PID_Speed->Unit_Interal_Motorpulse2=0;
PID_Speed->PIDOUT=0;
	//右轮单位时间捕获到的脉冲数
}
//角度环参数初始化
void PID_Angle_Init(PIDTypdDef*PID_Angle){
PID_Angle->err_last=0;
PID_Angle->err_now=0;
PID_Angle->Kd=0;
PID_Angle->Ki=0;
PID_Angle->Kp=0;
PID_Angle->target_val=0;
PID_Angle->actual_val=0;
PID_Angle->Unit_Interal_Motorpulse1=0;//左轮单位时间捕获到的脉冲数
PID_Angle->Unit_Interal_Motorpulse2=0;
PID_Angle->PIDOUT=0;
	//右轮单位时间捕获到的脉冲数
}
//位置环PID参数初始化
void PID_Location_Init(PIDTypdDef*PID_Location){
PID_Location->err_last=0;
PID_Location->err_now=0;
PID_Location->Kd=0;
PID_Location->Ki=0;
PID_Location->Kp=0;
PID_Location->target_val=0;
PID_Location->actual_val=0;
PID_Location->Interal_Motorpulse1=0;//左轮捕获到的总脉冲数
PID_Location->Interal_Motorpulse2=0;//右轮捕获到的总脉冲数
PID_Location->PIDOUT=0;

}
//速度环P,I,D的参数设置
void PID_Speed_SetK(PIDTypdDef*PID_Speed,float KP,float KI,float KD)
{
	PID_Speed->Kp =  KP ;
	PID_Speed->Ki =  KI ;
	PID_Speed->Kd =  KD ;	
}

void PID_Angle_SetK(PIDTypdDef*PID_Angle,float KP,float KI,float KD)
{
	PID_Angle->Kp =  KP ;
	PID_Angle->Ki =  KI ;
	PID_Angle->Kd =  KD ;	
}
void PID_Speed_SetKp(PIDTypdDef*PID_Speed,float KP)
{
	PID_Speed->Kp =  KP ;	
}
void PID_Speed_SetKi(PIDTypdDef*PID_Speed,float KI)
{
	PID_Speed->Ki =  KI ;	
}
void PID_Speed_SetKd(PIDTypdDef*PID_Speed,float KD)
{
	PID_Speed->Kd =  KD ;	
}
//位置环P,I,D的参数设置函数

void PID_Location_SetKp(PIDTypdDef*PID_Location,float KP)
{
	PID_Location->Kp =  KP ;	
}
void PID_Location_SetKi(PIDTypdDef*PID_Location,float KI)
{
	PID_Location->Ki =  KI ;	
}
void PID_Location_SetKd(PIDTypdDef*PID_Location,float KD)
{
	PID_Location->Kd =  KD ;	
}
//位置环最初未优化版PID计算，参数1是左右轮的选择，参数2是通过编码器记录的路程
float Location_pid_realize(PIDTypdDef*PID_Location,float Location_Actual_val){
//1、计算偏差
  PID_Location->err_now= PID_Location->target_val-Location_Actual_val;
	//2、累计偏差
  PID_Location->Error_Interal+=PID_Location->err_now;
	
	//3、积分限幅
  PID_Location->Error_Interal=(PID_Location->Error_Interal>2000)?PID_Location->Error_Interal=2000:((PID_Location->Error_Interal<-2000)?PID_Location->Error_Interal=-2000:PID_Location->Error_Interal);
    //4、pid计算
	PID_Location->actual_val=PID_Location->Kp*PID_Location->err_now+PID_Location->Ki*PID_Location->Error_Interal+PID_Location->Kd*(PID_Location->err_now-PID_Location->err_last);
	//5、误差交互
  PID_Location->err_last=PID_Location->err_now;

  return PID_Location->actual_val;
}
//角度环pd控制器
float Angle_pid_realize(PIDTypdDef*PID_Angle,float Angle_Actual_val){
//1、计算偏差
  PID_Angle->err_now= PID_Angle->target_val-Angle_Actual_val;
	//2、累计偏差
  PID_Angle->Error_Interal+=PID_Angle->err_now;
	
	//3、积分限幅
//  PID_Angle->Error_Interal=(PID_Angle->Error_Interal>2000)?PID_Angle->Error_Interal=2000:((PID_Location->Error_Interal<-2000)?PID_Location->Error_Interal=-2000:PID_Location->Error_Interal);
    //4、pid计算
	PID_Angle->actual_val=PID_Angle->Kp*PID_Angle->err_now+PID_Angle->Ki*PID_Angle->Error_Interal+PID_Angle->Kd*(PID_Angle->err_now-PID_Angle->err_last);
	//5、误差交互
  PID_Angle->err_last=PID_Angle->err_now;

  return PID_Angle->actual_val;
}
//速度环最初未优化版PID计算，参数1是左右轮的选择，参数2是速度的传入	
float Speed_pid_realize(PIDTypdDef*PID_Speed,float Speed_Actual_val){
//1、计算偏差
	
  PID_Speed->err_now= PID_Speed->target_val-Speed_Actual_val;
	//2、累计偏差
  PID_Speed->Error_Interal+=PID_Speed->err_now;
	 PID_Speed->Error_Interal=(PID_Speed->Error_Interal>6000)?PID_Speed->Error_Interal=6000:((PID_Speed->Error_Interal<-6000)?PID_Speed->Error_Interal=-6000:PID_Speed->Error_Interal);
	//3、pid计算
  PID_Speed->actual_val=PID_Speed->Kp*PID_Speed->err_now+PID_Speed->Ki*PID_Speed->Error_Interal+PID_Speed->Kd*(PID_Speed->err_now-PID_Speed->err_last);
	//4、误差交互
  PID_Speed->err_last=PID_Speed->err_now;
  return PID_Speed->actual_val;
}

//位置环输入为期望位置->左轮
float Location_Ring1_out(PIDTypdDef*PID_Location){
float expectSpeed=0;
PID_Location->actual_val=Interal_Motorpulse1;//速度环的实际位置为当前捕获的总脉冲数
expectSpeed=Location_pid_realize(&Motor_Left,Interal_Motorpulse1);//位置环PID输出值为左轮电机速度的期望速度
	//此处保留一个限制电机速度最大的函数
//	PID_Speed->target_val=(PID_Speed->target_val>160)?PID_Speed->target_val=160:(PID_Speed->target_val<-160)?PID_Speed->target_val=-160:PID_Speed->target_val>160;
	return expectSpeed;
}

float Location_Ring2_out(PIDTypdDef*PID_Location){
float expectSpeed=0;
PID_Location->actual_val=Interal_Motorpulse2;//速度环的实际位置为当前捕获的总脉冲数
expectSpeed=Location_pid_realize(&Motor_Right,Interal_Motorpulse2);//位置环PID输出值为右轮电机速度的期望速度
	//此处保留一个限制电机速度最大的函数
//	PID_Speed->target_val=(PID_Speed->target_val>160)?PID_Speed->target_val=160:(PID_Speed->target_val<-160)?PID_Speed->target_val=-160:PID_Speed->target_val>160;
	return expectSpeed;
}



//为了防止单片机计算不过来，就将其拆分成两个函数先计算一个再计算另外一个；
//输出左轮的期望PWM

//左轮转向环PD速度控制器

//右轮转向环PD速度控制器
void Set_Target_Left_position(float expect_dist){
Motor_Left.Dist1_expect=expect_dist;
}
void Set_Target_Right_position(float expect_dist){
Motor_Right.Dist2_expect=expect_dist;
}
void Set_Target_speed(PIDTypdDef*PID_Speed,float expect_speed){
PID_Speed->target_val=expect_speed;
}

void Set_Actual_speed(PIDTypdDef*PID_Speed,float actual_speed){
PID_Speed->actual_val=actual_speed;
}
void Set_Target_position(PIDTypdDef*PID_Location,float expect_dist){
PID_Location->target_val=expect_dist;
}
void Set_Target_angle(PIDTypdDef*PID_Angle,float expect_angle){
PID_Angle->target_val=expect_angle;
}
