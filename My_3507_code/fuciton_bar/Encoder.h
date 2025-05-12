#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "main.h"

//�����������
#define ENCODE_13X 13 		//����������
#define JIANSUBI 20     	//���ٱ�
#define BEIPIN 8            //��Ƶ
#define SAMPLE_TIME 0.01	//����ʱ��
#define CC (ENCODE_13X*JIANSUBI*BEIPIN*SAMPLE_TIME)

#define PI  3.1415f
#define RR  42.5f    		//���ְ뾶��λcm

/*�������˿ڶ�ȡ�궨��*/
#define Read_Encoder_A 	(DL_GPIO_readPins(Encoder_PORT,Encoder_A_PIN)==Encoder_A_PIN)?0:1//���� A��
#define Read_Encoder_B  (DL_GPIO_readPins(Encoder_PORT,Encoder_B_PIN)==Encoder_B_PIN)?0:1//���� B��
#define Read_Encoder_C 	(DL_GPIO_readPins(Encoder_PORT,Encoder_C_PIN)==Encoder_C_PIN)?0:1//���� A��
#define Read_Encoder_D 	(DL_GPIO_readPins(Encoder_PORT,Encoder_D_PIN)==Encoder_D_PIN)?0:1//���� B��

extern float Motor1_Speed;
extern float Motor2_Speed;
extern float Measure_Distance;

void Motor1_Get_Speed(void);
void Motor2_Get_Speed(void);
//�������е���ٶ�
void MEASURE_MOTORS_SPEED(void);

#endif
