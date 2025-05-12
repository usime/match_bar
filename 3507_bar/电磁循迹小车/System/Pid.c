#include "stm32f10x.h"                  // Device header
#include "Sensor.h"
extern float Kp,Ki,Kd;
float err;
float err_sum=0;
float err_diff;
float err_last=0;

extern int L_Duty;
extern int R_Duty;

void Angle_Pid(Sensor_Data *Sensor){
	err_last=err;
	err=Sensor->location;
	err_sum+=err;
	err_diff=err-err_last;
	L_Duty=Kp*err+Ki*err_sum+Kd*err_diff;
	R_Duty=Kp*err+Ki*err_sum+Kd*err_diff;
}


