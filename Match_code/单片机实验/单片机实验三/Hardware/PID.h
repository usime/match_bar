//PID
#ifndef __PID_H
#define __PID_H
typedef struct {
	float actual_val;
	float target_val;//Ŀ��ֵ
	volatile float err_now;//err[0]
	volatile float err_last;//err[-1]
	float Kp, Ki, Kd;
	volatile float Ki_index;//����ϵ��
	volatile float Error_Interal;
	float Interal_Motorpulse1;//左轮脉冲总数
	float Interal_Motorpulse2;//右轮脉冲总数
	uint16_t Unit_Interal_Motorpulse1;//左轮单位时间内收集到的脉冲数
	uint16_t Unit_Interal_Motorpulse2;//右轮单位时间内收集到的脉冲数
	float Dist1_expect;
	float Dist2_expect;
	float Speed1_expect;
	float Speed2_expect;
	float Angle1_expect;
	float Angle2_expect;
	float PIDOUT;
}PIDTypdDef;
extern PIDTypdDef Motor_Ctrl;
extern  int mypid[3];
void PID_Speed_SetK(PIDTypdDef*PID_Speed,float KP,float KI,float KD);
void PID_Speed_SetKp(PIDTypdDef*PID_Speed,float KP);
void PID_Speed_SetKi(PIDTypdDef*PID_Speed,float KI);
void PID_Speed_SetKd(PIDTypdDef*PID_Speed,float KD);
void PID_Location_SetK(PIDTypdDef*PID_Location,float KP,float KI,float KD);
void PID_Location_SetKp(PIDTypdDef*PID_Location,float KP);
void PID_Location_SetKi(PIDTypdDef*PID_Location,float KI);
void PID_Location_SetKd(PIDTypdDef*PID_Location,float KD);
float Location_pid_realize(PIDTypdDef*PID_Location,float Location_Actual_val);
float Speed_pid_realize(PIDTypdDef*PID_Speed,float Speed_Actual_val);
float Location_Ring2_out(PIDTypdDef*PID_Location);
float Location_Ring1_out(PIDTypdDef*PID_Location);
float Speed_Ring1_out(PIDTypdDef*PID_Speed);
float Speed_Ring2_out(PIDTypdDef*PID_Speed);
void PID_Angle_Init(PIDTypdDef*PID_Angle);
float Angle_pid_realize(PIDTypdDef*PID_Angle,float Angle_Actual_val);
void Set_Target_angle(PIDTypdDef*PID_Angle,float expect_angle);
float Location_Speed_ring_turn(void);
void PID_Angle_SetK(PIDTypdDef*PID_Angle,float KP,float KI,float KD);
void Set_Target_position(PIDTypdDef*PID_Location,float expect_dist);
extern PIDTypdDef* PID_Location;
void Set_Target_speed(PIDTypdDef*PID_Speed,float expect_speed);
extern PIDTypdDef* PID_Location;
extern PIDTypdDef* PID_Speed;
void PID_Speed_Init(PIDTypdDef*PID_Speed);
void PID_Location_Init(PIDTypdDef*PID_Location);
float Location_SpeedRing1(PIDTypdDef*PID_Speed,PIDTypdDef*PID_Location,float Location_Actual_val,float Speed_Actual_val);
float Location_SpeedRing2(PIDTypdDef*PID_Speed,PIDTypdDef*PID_Location,float Location_Actual_val,float Speed_Actual_val);
void Set_Actual_speed(PIDTypdDef*PID_Speed,float actual_speed);
void Set_Target_Left_position(float expect_dist);
void Set_Target_Right_position(float expect_dist);
void Set_Target_position(PIDTypdDef*PID_Location,float expect_dist);
#endif



