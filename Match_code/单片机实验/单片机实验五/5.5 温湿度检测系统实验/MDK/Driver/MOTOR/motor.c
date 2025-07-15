#include "motor.h"

void MOTOR_init(void)						//电机GPIO引脚初始化
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHBxPeriph_MOTOR_EN1 | RCC_AHBxPeriph_MOTOR_IN1 | RCC_AHBxPeriph_MOTOR_IN2, ENABLE);	//使能端口时钟
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin   = MOTOR_EN1_GPIO_PIN;
	GPIO_Init(MOTOR_EN1_GPIO_PORT, &GPIO_InitStructure); //初始化GPIO	EN1
	GPIO_ResetBits(MOTOR_EN1_GPIO_PORT,MOTOR_EN1_GPIO_PIN);	

	GPIO_InitStructure.GPIO_Pin   = MOTOR_IN1_GPIO_PIN;
	GPIO_Init(MOTOR_IN1_GPIO_PORT, &GPIO_InitStructure); //初始化GPIO	IN1
	GPIO_ResetBits(MOTOR_IN1_GPIO_PORT,MOTOR_IN1_GPIO_PIN);		

	GPIO_InitStructure.GPIO_Pin   = MOTOR_IN2_GPIO_PIN;
	GPIO_Init(MOTOR_IN2_GPIO_PORT, &GPIO_InitStructure); //初始化GPIO	IN2
	GPIO_ResetBits(MOTOR_IN2_GPIO_PORT,MOTOR_IN2_GPIO_PIN);		
}


void MOTOR_control(unsigned int control)			//电机控制
{
	switch(control)
	{
		case 0:
			//电机停止
			MOTOR_EN1 = 0;
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			break;
		case 1:
			//电机反转
			MOTOR_EN1 = 1;
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 0;
			break;
		case 2:
			//电机正转
			MOTOR_EN1 = 1;
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 1;
			break;
	}
}

void MOTOR_PWM_init(unsigned int frequency,unsigned int zkb)	//传递频率参数	占空比参数	
{
	GPIO_InitTypeDef  GPIO_InitStructure;					//定义一个初始化IO口配置的结构体
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_PWM | RCC_APB2Periph_AFIO,ENABLE);	//使能端口时钟和复用时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWM,ENABLE);				//使能TIM时钟

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO口速度为50MHz
	GPIO_InitStructure.GPIO_Pin = PWM_GPIO_PIN;							//端口配置
	GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStructure);						//根据设定参数初始化GPIOA		
	
	//写初值（预分频寄存器PSC，自动重装载寄存器ARR,计数模数CR1  .BIT4）
	TIM_InitStructure.TIM_Prescaler = SystemCoreClock/(frequency*100)-1;     
	TIM_InitStructure.TIM_Period = 100-1; 
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;			//向上计数
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(PWM_TIM,&TIM_InitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//设置PWM模数1  CNT<CCRX  有效
	TIM_OCInitStructure.TIM_Pulse = zkb;							//写入CCRx的数值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//有效输出极性高
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//输出oc使能
	PWM_TIM_OCXInit(PWM_TIM,&TIM_OCInitStructure);
	
	PWM_TIM_OCXPreloadConfig(PWM_TIM,TIM_OCPreload_Enable);
	TIM_Cmd(PWM_TIM,ENABLE);
}



void MOTOR_PWM_control(unsigned int control,unsigned int zkb)			//电机PWM控制
{
	switch(control)
	{
		case 0:
			//电机停止
			MOTOR_EN1 = 0;
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			break;
		case 1:
			//电机反转
			MOTOR_EN1 = 1;
			PWM_TIM_SetCompareX(PWM_TIM,zkb);
			MOTOR_IN2 = 0;
			break;
		case 2:
			//电机正转
			MOTOR_EN1 = 1;
			PWM_TIM_SetCompareX(PWM_TIM,100-zkb);
			MOTOR_IN2 = 1;
			break;
	}
}

void MOTOR_PLU_init(unsigned int ms)		//电机脉冲捕获初始化，这里采用定时器来处理，定时和捕获功能：注意：也可以采用定时加外部中断来处理。
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_PLU|RCC_APB2Periph_AFIO,ENABLE);								//使能端口时钟和复用时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PLU, ENABLE);													//时钟使能
	
	GPIO_InitStructure.GPIO_Pin = PLU_GPIO_PIN;															//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;														//输入
	GPIO_Init(PLU_GPIO_PORT, &GPIO_InitStructure);														//根据设定参数初始化GPIO
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = 10*ms-1;															//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =SystemCoreClock/10000-1;										//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;												//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;											//TIM向上计数模式
	TIM_TimeBaseInit(PLU_TIM, &TIM_TimeBaseStructure);														//根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ICInitStructure.TIM_Channel = PLU_TIM_Channel_X;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;											//上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;										//映射到TI1上
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;												//配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x00;															//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(PLU_TIM,&TIM_ICInitStructure);	
	
	TIM_ITConfig(PLU_TIM,PLU_TIM_IT_CCX | TIM_IT_Update,ENABLE);										//使能中断允许位

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = PLU_TIMX_IRQn;													//TIM中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;											//抢占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;													//响应优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;														//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);																		//初始化NVIC寄存器

	TIM_Cmd(PLU_TIM, ENABLE);																			//使能TIM			 
}


unsigned long PLU_count = 0; 	//全局定时采集的脉冲信号
unsigned char PLU_flag  = 0; 	//全局定时采集的脉冲信号完成标志位
//定时器2中断服务程序
void PLU_TIMX_IRQHandler(void)   
{
	static unsigned long tim_count = 0;
	if(TIM_GetITStatus(PLU_TIM,PLU_TIM_IT_CCX))																//捕获中断
	{
		TIM_ClearITPendingBit(PLU_TIM,PLU_TIM_IT_CCX);
		tim_count++;
	}
	if(TIM_GetITStatus(PLU_TIM,TIM_IT_Update))																//更新中断
	{
		TIM_ClearITPendingBit(PLU_TIM,TIM_IT_Update);
		PLU_count = tim_count;
		PLU_flag = 1;
		tim_count = 0;
	}
}
