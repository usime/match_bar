/***
	*******************************************************************************************************************************************************************
	* @file    Encoder.c
	* @version V2.1
	* @date    2024-7-22
	* @author  ����	
	* @brief   MSPM0G3507С��PID����ͨ��ģ��
   *************************************************************************************************
   *  @description
	*	
	*  ֻ��Ҫ����MEASURE_MOTORS_SPEED()��������Motor1_Speed��Motor2_SpeedΪ���ʵ���ٶ�
   *
>>>>> ����˵����δ������������ת�����������״��룬��Ҷ����ҹ�����Ϊ���꣬�뱣�ֺ��Լ��ĳ��ģ��ڴˣ��������ҵĸ�л
	*************************************************************************************************************************************************************
***/
#include "Encoder.h"
#include "mpu6050.h"
#include "key.h"

int32_t Motor1_Encoder_Value=0;
int32_t Motor2_Encoder_Value=0;

float Motor1_Speed = 0;
float Motor2_Speed = 0;
//�ⲿ�ж϶�ȡ��������ֵ
void GROUP1_IRQHandler(void){
	if(DL_Interrupt_getStatusGroup(DL_INTERRUPT_GROUP_1,DL_INTERRUPT_GROUP1_GPIOB)){
		uint32_t Encoder_GPIO_Int = DL_GPIO_getEnabledInterruptStatus(Encoder_PORT,Encoder_A_PIN | Encoder_B_PIN | Encoder_C_PIN | Encoder_D_PIN);
		//ͨ��1 ����A��
		if ((Encoder_GPIO_Int & Encoder_A_PIN) == Encoder_A_PIN){
			DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_A_PIN);
			if(Read_Encoder_A == 1){ //������
				if (Read_Encoder_B == 0){
					Motor1_Encoder_Value++;
				}
				else if(Read_Encoder_B == 1){
					Motor1_Encoder_Value--;
				}
			}
			else if(Read_Encoder_A == 0){//�½���
				if (Read_Encoder_B == 0){
					Motor1_Encoder_Value--;
				}
				else if(Read_Encoder_B == 1){
					Motor1_Encoder_Value++;
				}
			}
		}		
		//ͨ��2 ����B��
		if ((Encoder_GPIO_Int & Encoder_B_PIN) == Encoder_B_PIN){
			DL_GPIO_clearInterruptStatus(Encoder_PORT,Encoder_B_PIN);
			if(Read_Encoder_B == 1){ //������
				if (Read_Encoder_A == 0){
					Motor1_Encoder_Value--;
				}
				else if (Read_Encoder_A == 1){
					Motor1_Encoder_Value++;
				}
			}
			else if(Read_Encoder_B == 0){//�½���
				if (Read_Encoder_A == 0){
					Motor1_Encoder_Value++;
				}
				else if (Read_Encoder_A == 1){
					Motor1_Encoder_Value--;
				}
			}
		}
		//ͨ��3 ����A��		
		if ((Encoder_GPIO_Int & Encoder_C_PIN) == Encoder_C_PIN){
			DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_C_PIN);
			if(Read_Encoder_C == 1){ //������
				if (Read_Encoder_D  == 0){
					Motor2_Encoder_Value++;
				}
				else if (Read_Encoder_D  == 1){
					Motor2_Encoder_Value--;
				}
			}
			else if(Read_Encoder_C == 0){//�½���
				if (Read_Encoder_D  == 0){
					Motor2_Encoder_Value--;
				}
				else if (Read_Encoder_D  == 1){
					Motor2_Encoder_Value++;
				}
			}
		}		
		//ͨ��4 ����B��
		if ((Encoder_GPIO_Int & Encoder_D_PIN) == Encoder_D_PIN){
			DL_GPIO_clearInterruptStatus(Encoder_PORT,Encoder_D_PIN);
			if(Read_Encoder_D == 1){ //������
				if (Read_Encoder_C  == 0){
					Motor2_Encoder_Value--;
				}
				else if (Read_Encoder_C  == 1){
					Motor2_Encoder_Value++;
				}
			}
			else if(Read_Encoder_D == 0){//�½���
				if (Read_Encoder_C  == 0){
					Motor2_Encoder_Value++;
				}
				else if (Read_Encoder_C  == 1){
					Motor2_Encoder_Value--;
				}
			}
		}
	}
}

void Motor1_Get_Speed(void){
    short Encoder_TIM = 0;
    float Speed = 0;
    Encoder_TIM= Motor1_Encoder_Value;
    Motor1_Encoder_Value=0;
    Speed =(float)Encoder_TIM/(CC)*PI*RR;
    Motor1_Speed = -Speed;
}

void Motor2_Get_Speed(void){
    short Encoder_TIM = 0;
    float Speed = 0;
    Encoder_TIM= Motor2_Encoder_Value;
    Motor2_Encoder_Value=0;
    Speed =(float)Encoder_TIM/(CC)*PI*RR;
    Motor2_Speed = Speed;
}
float Motor1_Lucheng,Motor2_Lucheng;
float Measure_Distance = 0;
//�������е���ٶ�
void MEASURE_MOTORS_SPEED(void){
	Motor1_Get_Speed();Motor2_Get_Speed();
	Motor1_Lucheng += Motor1_Speed*SAMPLE_TIME;//·���ۼ�
	Motor2_Lucheng += Motor2_Speed*SAMPLE_TIME;//·���ۼ�
	Measure_Distance = Motor1_Lucheng/2.0 +Motor2_Lucheng/2.0;

}
