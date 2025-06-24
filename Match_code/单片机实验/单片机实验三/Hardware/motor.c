#include "stm32f10x.h"                  // Device header
#include "Motor.h"
/**
  * ��    ����ֱ�������ʼ��
  * ��    ������
  * �� �� ֵ����
  */
#define PWM_MAX 3070
//ö�ٱ����������ֿ���ģʽ
int Motor_en_flag;
 Motor_Ctrl_choose Current_State;
extern int Flag_Motor_en;
const char* get_state_name(Current_State) {
    switch(Current_State) {
        case Forward:    return "Forward" ;
        case Back: return "Back";
        case Stop:   return "Stop";
      
    }
}
void Motor_En_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//����GPIOA��ʱ��
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						//��PA4��PA5���ų�ʼ��Ϊ�������												//��ʼ��ֱ������ĵײ�PWM
}

void Motor_Init_Right(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//����GPIOA��ʱ��
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//��PA4��PA5���ų�ʼ��Ϊ�������											//��ʼ��ֱ������ĵײ�PWM
}
//�������ƺ��� ����������ѡ��ģʽ ǰ�� ���� ֹͣ
void Motor_Left_Control(Motor_Ctrl_choose Options){
if(Options==Forward){
GPIO_SetBits(GPIOA,GPIO_Pin_11);
GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}
else if(Options==Back){
GPIO_ResetBits(GPIOA,GPIO_Pin_11);
GPIO_SetBits(GPIOA,GPIO_Pin_12);
}
else if(Options==Stop){
GPIO_ResetBits(GPIOA,GPIO_Pin_11);
GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}
}
//�ҵ�����ƺ��� ����������ѡ��ģʽ ǰ�� ���� ֹͣ
void Motor_Right_Control(Motor_Ctrl_choose Options){
if(Options==Forward){
GPIO_SetBits(GPIOB,GPIO_Pin_14);
GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}
else if(Options==Back){
GPIO_ResetBits(GPIOB,GPIO_Pin_14);
GPIO_SetBits(GPIOB,GPIO_Pin_15);
}
else if(Options==Stop){
GPIO_ResetBits(GPIOB,GPIO_Pin_14);
GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}
}
//���ҵ����PWMװ�ء�����ֵ�������޷�����
void Load_Motor_pwm(int Motor1_pwm,int Motor2_pwm){
if(Motor1_pwm>0){
Motor1_pwm=(Motor1_pwm>0)?Motor1_pwm:(-Motor1_pwm);//�����ж�
Motor2_pwm=(Motor2_pwm>0)?Motor2_pwm:(-Motor2_pwm);//�����ж�
Motor1_pwm=(Motor1_pwm>=PWM_MAX)?PWM_MAX:Motor1_pwm;//���аٷ�֮60ռ�ձȻ����޷�
Motor2_pwm=(Motor2_pwm>=PWM_MAX)?PWM_MAX:Motor2_pwm;//���аٷ�֮60ռ�ձȻ����޷�
TIM_SetCompare4(TIM3, Motor1_pwm);	//���ֶ�ʱ��4ͨ��1
TIM_SetCompare1(TIM3, Motor2_pwm);	//���ֶ�ʱ��4ͨ��2
Motor_Ctrl_choose(Forward);
}
else{
Motor1_pwm=(Motor1_pwm>0)?Motor1_pwm:(-Motor1_pwm);//�����ж�
Motor2_pwm=(Motor2_pwm>0)?Motor2_pwm:(-Motor2_pwm);//�����ж�
Motor1_pwm=(Motor1_pwm>=PWM_MAX)?PWM_MAX:Motor1_pwm;//���аٷ�֮60ռ�ձȻ����޷�
Motor2_pwm=(Motor2_pwm>=PWM_MAX)?PWM_MAX:Motor2_pwm;//���аٷ�֮60ռ�ձȻ����޷�
TIM_SetCompare4(TIM3, Motor1_pwm);	//���ֶ�ʱ��4ͨ��1
TIM_SetCompare1(TIM3, Motor2_pwm);	//���ֶ�ʱ��4ͨ��2
Motor_Ctrl_choose(Back);
}
}



//ֱ��ʹ�ܺ�ʧ�ܺ��������и��õ�ɲ������Ч��
void Set_Motor_Enable(){
GPIO_SetBits(GPIOA,GPIO_Pin_12);
Motor_en_flag=1;
}

void Set_Motor_Disable(){
GPIO_ResetBits(GPIOA,GPIO_Pin_12);
Motor_en_flag=0;
}

/**
  * ��    ����ֱ����������ٶ�
  * ��    ����Speed Ҫ���õ��ٶȣ���Χ��-100~100
  * �� �� ֵ����
  */




