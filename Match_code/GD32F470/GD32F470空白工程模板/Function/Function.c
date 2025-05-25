/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���Function.c
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/


/************************* ͷ�ļ� *************************/

#include "Function.h"

/************************* �궨�� *************************/


/************************ �������� ************************/


/************************ �������� ************************/



/************************************************************ 
 * Function :       System_Init
 * Comment  :       ���ڳ�ʼ��MCU
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void System_Init(void)
{
	systick_config();     // ʱ������

	LED_Init();          // LED ��ʼ��
}
/************************************************************ 
 * Function :       Init_LED_Stat
 * Comment  :       ϵͳ��ʼ��ʱ��LED��ʾ״̬
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-03-10 V0.1 original
************************************************************/

void Init_LED_Stat(void)
{
	//����˵����
	//��ϵͳ�ϵ��ִ������ĳ���
	//LED1~4���1�����ε�����ض�
	//����ָʾϵͳ�ϵ������û�����Ķ���
	
	LED1_ON();
    delay_1ms(1000);
	LED2_ON();
    delay_1ms(1000);
	LED3_ON();
    delay_1ms(1000);
	LED4_ON();
    delay_1ms(1000);
	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
	LED4_OFF();

}
/************************************************************ 
 * Function :       UsrFunction
 * Comment  :       �û�������: LED1��˸
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void UsrFunction(void)
{
	Init_LED_Stat();
	
	while(1)
	{
		LED1_ON();
		delay_1ms(500);
		LED1_OFF();
		delay_1ms(500);
	
	}
}


/****************************End*****************************/

