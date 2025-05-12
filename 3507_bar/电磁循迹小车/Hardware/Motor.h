#ifndef __MOTOR_H

void Motor_Init(void);
void SetDuty(int16_t Left_Speed,int16_t Right_Speed);
void Motor_Pin_Init(void);
#define __MOTOR_H
#endif
