#include "motor.h"

void MOTOR_init(void)						//���GPIO���ų�ʼ��
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_MOTOR_EN1 | RCC_AHBxPeriph_MOTOR_IN1 | RCC_AHBxPeriph_MOTOR_IN2, ENABLE);	//ʹ�ܶ˿�ʱ��
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin   = MOTOR_EN1_GPIO_PIN;
	GPIO_Init(MOTOR_EN1_GPIO_PORT, &GPIO_InitStructure); //��ʼ��GPIO	EN1
	GPIO_ResetBits(MOTOR_EN1_GPIO_PORT,MOTOR_EN1_GPIO_PIN);	

	GPIO_InitStructure.GPIO_Pin   = MOTOR_IN1_GPIO_PIN;
	GPIO_Init(MOTOR_IN1_GPIO_PORT, &GPIO_InitStructure); //��ʼ��GPIO	IN1
	GPIO_ResetBits(MOTOR_IN1_GPIO_PORT,MOTOR_IN1_GPIO_PIN);		

	GPIO_InitStructure.GPIO_Pin   = MOTOR_IN2_GPIO_PIN;
	GPIO_Init(MOTOR_IN2_GPIO_PORT, &GPIO_InitStructure); //��ʼ��GPIO	IN2
	GPIO_ResetBits(MOTOR_IN2_GPIO_PORT,MOTOR_IN2_GPIO_PIN);		
}


void MOTOR_control(unsigned int control)			//�������
{
	switch(control)
	{
		case 0:
			//���ֹͣ
			MOTOR_EN1 = 0;
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			break;
		case 1:
			//�����ת
			MOTOR_EN1 = 1;
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 0;
			break;
		case 2:
			//�����ת
			MOTOR_EN1 = 1;
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 1;
			break;
	}
}

void MOTOR_PWM_init(unsigned int frequency,unsigned int zkb)	//����Ƶ�ʲ���	ռ�ձȲ���	
{
	GPIO_InitTypeDef  GPIO_InitStructure;					//����һ����ʼ��IO�����õĽṹ��
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_PWM | RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܶ˿�ʱ�Ӻ͸���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWM,ENABLE);				//ʹ��TIMʱ��

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Pin = PWM_GPIO_PIN;							//�˿�����
	GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStructure);						//�����趨������ʼ��GPIOA		
	
	//д��ֵ��Ԥ��Ƶ�Ĵ���PSC���Զ���װ�ؼĴ���ARR,����ģ��CR1  .BIT4��
	TIM_InitStructure.TIM_Prescaler = SystemCoreClock/(frequency*100)-1;     
	TIM_InitStructure.TIM_Period = 100-1; 
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;			//���ϼ���
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(PWM_TIM,&TIM_InitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//����PWMģ��1  CNT<CCRX  ��Ч
	TIM_OCInitStructure.TIM_Pulse = zkb;							//д��CCRx����ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//��Ч������Ը�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//���ocʹ��
	PWM_TIM_OCXInit(PWM_TIM,&TIM_OCInitStructure);
	
	PWM_TIM_OCXPreloadConfig(PWM_TIM,TIM_OCPreload_Enable);
	TIM_Cmd(PWM_TIM,ENABLE);
}



void MOTOR_PWM_control(unsigned int control,unsigned int zkb)			//���PWM����
{
	switch(control)
	{
		case 0:
			//���ֹͣ
			MOTOR_EN1 = 0;
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			break;
		case 1:
			//�����ת
			MOTOR_EN1 = 1;
			PWM_TIM_SetCompareX(PWM_TIM,zkb);
			MOTOR_IN2 = 0;
			break;
		case 2:
			//�����ת
			MOTOR_EN1 = 1;
			PWM_TIM_SetCompareX(PWM_TIM,100-zkb);
			MOTOR_IN2 = 1;
			break;
	}
}

void MOTOR_PLU_init(unsigned int ms)		//������岶���ʼ����������ö�ʱ����������ʱ�Ͳ����ܣ�ע�⣺Ҳ���Բ��ö�ʱ���ⲿ�ж�������
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_PLU|RCC_APB2Periph_AFIO,ENABLE);								//ʹ�ܶ˿�ʱ�Ӻ͸���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PLU, ENABLE);													//ʱ��ʹ��
	
	GPIO_InitStructure.GPIO_Pin = PLU_GPIO_PIN;															//�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;														//����
	GPIO_Init(PLU_GPIO_PORT, &GPIO_InitStructure);														//�����趨������ʼ��GPIO
	
	//��ʱ��TIM2��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 10*ms-1;															//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =SystemCoreClock/10000-1;										//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;												//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;											//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(PLU_TIM, &TIM_TimeBaseStructure);														//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ICInitStructure.TIM_Channel = PLU_TIM_Channel_X;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;											//�����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;										//ӳ�䵽TI1��
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;												//���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICFilter = 0x00;															//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(PLU_TIM,&TIM_ICInitStructure);	
	
	TIM_ITConfig(PLU_TIM,PLU_TIM_IT_CCX | TIM_IT_Update,ENABLE);										//ʹ���ж�����λ

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = PLU_TIMX_IRQn;													//TIM�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;											//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;													//��Ӧ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;														//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);																		//��ʼ��NVIC�Ĵ���

	TIM_Cmd(PLU_TIM, ENABLE);																			//ʹ��TIM			 
}


unsigned long PLU_count = 0; 	//ȫ�ֶ�ʱ�ɼ��������ź�
unsigned char PLU_flag  = 0; 	//ȫ�ֶ�ʱ�ɼ��������ź���ɱ�־λ
//��ʱ��2�жϷ������
void PLU_TIMX_IRQHandler(void)   
{
	static unsigned long tim_count = 0;
	if(TIM_GetITStatus(PLU_TIM,PLU_TIM_IT_CCX))																//�����ж�
	{
		TIM_ClearITPendingBit(PLU_TIM,PLU_TIM_IT_CCX);
		tim_count++;
	}
	if(TIM_GetITStatus(PLU_TIM,TIM_IT_Update))																//�����ж�
	{
		TIM_ClearITPendingBit(PLU_TIM,TIM_IT_Update);
		PLU_count = tim_count;
		PLU_flag = 1;
		tim_count = 0;
	}
}
