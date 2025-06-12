/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���RTC.c
 * ����: Qiao Qin @ GigaDevice
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Qiao Qin     2025/4/20     V0.01    original
************************************************************/


#include "RTC.h"
#include "HeaderFiles.h"

#define RTC_CLOCK_SOURCE_LXTAL		// ����RTCʱ��Դ��ʹ���ⲿ32.768KHz
#define BKP_VALUE    0x32F0



rtc_parameter_struct   rtc_initpara;//RTC��ʼ���ṹ��
rtc_alarm_struct  rtc_alarm;//�������ýṹ��
__IO uint32_t prescaler_a = 0, prescaler_s = 0;//Ԥ��Ƶ��
uint32_t RTCSRC_FLAG = 0;



AdjustState current_state = ADJ_YEAR;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_Init(void)
{
    printf("\n\r  ****************** RTC calendar demo ******************\n\r");

    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);//ʹ�ܵ�Դ����Ԫ
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();//����������д����
	
    rtc_pre_config();//RTCԤ����
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    /* check if RTC has aready been configured */
	//�ж��Ƿ���Ҫ��������RTC
    if((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG)){
        /* backup data register value is not correct or not yet programmed
        or RTC clock source is not configured (when the first time the program 
        is executed or data in RCU_BDCTL is lost due to Vbat feeding) */
        rtc_setup();//������RTC��ʼ��
    }else{
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_PORRST)){
            printf("power on reset occurred....\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_EPRST)){
            printf("external reset occurred....\n\r");
        }
        printf("no need to configure RTC....\n\r");

        rtc_show_time();
    }
    rcu_all_reset_flag_clear();
}

/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_pre_config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC32K)
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);

          prescaler_s = 0x13F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);//�����ⲿ32.768KHz����
          rcu_osci_stab_wait(RCU_LXTAL);//�ȴ������ȶ�
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);//����ʱ��Դ

          prescaler_s = 0xFF;//ͬ����Ƶ255+1
          prescaler_a = 0x7F;//�첽��Ƶ127+1
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);//ʹ��
    rtc_register_sync_wait();//�ȴ�RTC�Ĵ���ͬ�����
}

/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_setup(void)
{
    /* setup RTC time value */
	uint32_t tmp_year = 0xFF, tmp_month = 0xFF, tmp_day = 0xFF;
    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

    rtc_initpara.factor_asyn = prescaler_a;//��֮ǰ��ķ�Ƶֵ
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = 0x16;//Ĭ�����2016
    rtc_initpara.day_of_week = RTC_SATURDAY;//Ĭ������
    rtc_initpara.month = RTC_APR;//Ĭ������
    rtc_initpara.date = 0x30;//30��
    rtc_initpara.display_format = RTC_24HOUR;//24Сʱ��
    rtc_initpara.am_pm = RTC_AM;

    /* current time input */
    printf("=======Configure RTC Time========\n\r");
	printf("  please set the last two digits of current year:\n\r");
    while(tmp_year == 0xFF) {
        tmp_year = usart_input_threshold(99);//��������
        rtc_initpara.year = tmp_year;
    }
    printf("  20%0.2x\n\r", tmp_year);

    printf("  please input month:\n\r");
    while(tmp_month == 0xFF) {
        tmp_month = usart_input_threshold(12);
        rtc_initpara.month = tmp_month;
    }
    printf("  %0.2x\n\r", tmp_month);

    printf("  please input day:\n\r");
    while(tmp_day == 0xFF) {
        tmp_day = usart_input_threshold(31);
        rtc_initpara.date = tmp_day;
    }
    printf("  %0.2x\n\r", tmp_day);
		
	
    printf("  please input hour:\n\r");
    while (0xFF == tmp_hh){
        tmp_hh = usart_input_threshold(23);
        rtc_initpara.hour = tmp_hh;
    }
    printf("  %0.2x\n\r", tmp_hh);

    printf("  please input minute:\n\r");
    while (0xFF == tmp_mm){
        tmp_mm = usart_input_threshold(59);
        rtc_initpara.minute = tmp_mm;
    }
    printf("  %0.2x\n\r", tmp_mm);

    printf("  please input second:\n\r");
    while (0xFF == tmp_ss){
        tmp_ss = usart_input_threshold(59);
        rtc_initpara.second = tmp_ss;
    }
    printf("  %0.2x\n\r", tmp_ss);

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){
        printf("\n\r** RTC time configuration failed! **\n\r");
    }else{
        printf("\n\r** RTC time configuration success! **\n\r");
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
    }
/*
//     setup RTC alarm 
//    tmp_hh = 0xFF;
//    tmp_mm = 0xFF;
//    tmp_ss = 0xFF;

//    rtc_alarm_disable(RTC_ALARM0);
//    printf("=======Input Alarm Value=======\n\r");
//    rtc_alarm.alarm_mask = RTC_ALARM_DATE_MASK|RTC_ALARM_HOUR_MASK|RTC_ALARM_MINUTE_MASK;
//    rtc_alarm.weekday_or_date = RTC_ALARM_DATE_SELECTED;
//    rtc_alarm.alarm_day = 0x31;
//    rtc_alarm.am_pm = RTC_AM;

//     RTC alarm input 
//    printf("  please input Alarm Hour:\n\r");
//    while (0xFF == tmp_hh){
//        tmp_hh = usart_input_threshold(23);
//        rtc_alarm.alarm_hour = tmp_hh;
//    }
//    printf("  %0.2x\n\r", tmp_hh);

//    printf("  Please Input Alarm Minute:\n\r");
//    while (0xFF == tmp_mm){
//        tmp_mm = usart_input_threshold(59);
//        rtc_alarm.alarm_minute = tmp_mm;
//    }
//    printf("  %0.2x\n\r", tmp_mm);

//    printf("  Please Input Alarm Second:\n\r");
//    while (0xFF == tmp_ss){
//        tmp_ss = usart_input_threshold(59);
//        rtc_alarm.alarm_second = tmp_ss;
//    }
//    printf("  %0.2x", tmp_ss);

//     RTC alarm configuration 
//    rtc_alarm_config(RTC_ALARM0,&rtc_alarm);
//    printf("\n\r** RTC Set Alarm Success!  **\n\r");
//    rtc_show_alarm();

//    rtc_interrupt_enable(RTC_INT_ALARM0);
//    rtc_alarm_enable(RTC_ALARM0);
*/
}

// ���뷶Χ��0-99��������Χ�����д���
uint8_t dec2bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}


/*!
    \brief      display the current time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_time(void)
{
//    uint32_t time_subsecond = 0;  ����Դ������뼶��ʾ
//    uint8_t subsecond_ss = 0,subsecond_ts = 0,subsecond_hs = 0;

    rtc_current_time_get(&rtc_initpara);//���ÿ⺯����Ӳ���Ĵ�����ȡʱ��
		//������������rtc_initpara��ֵ
    /* ��������߼�get the subsecond value of current time, and convert it into fractional format */
//    time_subsecond = rtc_subsecond_get();
//    subsecond_ss=(1000-(time_subsecond*1000+1000)/400)/100;
//    subsecond_ts=(1000-(time_subsecond*1000+1000)/400)%100/10;
//    subsecond_hs=(1000-(time_subsecond*1000+1000)/400)%10;
	OLED_Printf(0,0,12,"20%0.2x-%0.2x-%0.2x", 
           rtc_initpara.year, rtc_initpara.month, rtc_initpara.date);
	OLED_Printf(0,13,12,"  %0.2x:%0.2x:%0.2x \r\n",
           rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
	OLED_Refresh();
    printf("\r\nCurrent time: 20%0.2x-%0.2x-%0.2x", 
           rtc_initpara.year, rtc_initpara.month, rtc_initpara.date);

    printf(" : %0.2x:%0.2x:%0.2x \r\n", 
           rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
}

void rtc_show_EXchange_time(void)
{
    rtc_current_time_get(&rtc_initpara);//���ÿ⺯����Ӳ���Ĵ�����ȡʱ��
	OLED_Printf(0,0,12,"20%02d-%02d-%02d", 
           rtc_initpara.year, rtc_initpara.month, rtc_initpara.date);
	OLED_Printf(0,13,12,"  %02d:%02d:%02d \r\n",
           rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
		OLED_Refresh();
    printf("\r\nCurrent time: 20%02d-%02d-%02d", 
           rtc_initpara.year, rtc_initpara.month, rtc_initpara.date);

    printf(" : %02d:%02d:%02d \r\n", 
           rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
	OLED_Refresh();
}

/*!
    \brief      display the alram value
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_alarm(void)
{
    rtc_alarm_get(RTC_ALARM0,&rtc_alarm);
    printf("The alarm: %0.2x:%0.2x:%0.2x \n\r", rtc_alarm.alarm_hour, rtc_alarm.alarm_minute,
           rtc_alarm.alarm_second);
}

/*!
    \brief      get the input character string and check if it is valid
    \param[in]  none
    \param[out] none
    \retval     input value in BCD mode
*/



uint8_t usart_input_threshold(uint32_t value) {
    uint8_t tmp[2] = {0};
	int i=0;
	while(i < 2) {
        if(data_ready) {
            tmp[i] = data_recv;
            data_ready = 0;
            
            if(tmp[i] >= 0x30 && tmp[i] <= 0x39) {
					i++; // ����������
			}
		}       
    }
	//��ֹ����
    uint8_t decimal = (tmp[0]-0x30)*10 + (tmp[1]-0x30);
    if(decimal > value) {
        printf("\n\rInput exceed %d\n\r", value);
        return 0xFF;
    }
    return ((tmp[0]-0x30)<<4) | (tmp[1]-0x30); // ��ȷBCD����
}


// ������ֵ��Χ[min,max]
uint8_t CLAMP(uint8_t value, uint8_t min, int32_t max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// ѭ����ֵ��Χ[min,max]
uint8_t CYCLIC(uint8_t value, uint8_t min, uint8_t max) {
    if (value < min) return max;
    if (value > max) return min;
    return value;
}


// ��ȡ�����������
uint8_t get_max_day(uint16_t year, uint8_t month) {
    const uint8_t days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if(month == 2 && ((year%4==0 && year%100!=0) || year%400==0))
        return 29;
    return days[month-1];
}


//// Ӧ��RTC�޸�
//void apply_rtc_changes(void) {
//    /* enable PMU clock */
//    rcu_periph_clock_enable(RCU_PMU);//ʹ�ܵ�Դ����Ԫ
//    /* enable the access of the RTC registers */
//    pmu_backup_write_enable();//����������д����
//    
//    /* ����RTC */
//	rtc_current_time_get(&rtc_initpara);//���ÿ⺯����Ӳ���Ĵ�����ȡʱ��
//    if(ERROR == rtc_init(&rtc_initpara)) {
//        OLED_Printf(0,0,16,"RTC config failed!\n");
//    } else {
////        OLED_Printf(0,0,12," 20%02d-%02d-%02d %02d:%02d:%02d\n", 
////              rtc_initpara.year, rtc_initpara.month, rtc_initpara.date,
////              rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
//        rtc_show_time();
//        RTC_BKP0 = BKP_VALUE;
//    }
////	OLED_Refresh();
//}



void apply_rtc_changes(void) {
//		rtc_initpara.factor_asyn = prescaler_a;//��֮ǰ��ķ�Ƶֵ
//		rtc_initpara.factor_syn = prescaler_s;
//		rtc_initpara.year = 0x16;//Ĭ�����2016
//		rtc_initpara.day_of_week = RTC_SATURDAY;//Ĭ������
//		rtc_initpara.month = RTC_APR;//Ĭ������
//		rtc_initpara.date = 0x30;//30��
//		rtc_initpara.display_format = RTC_24HOUR;//24Сʱ��
//		rtc_initpara.am_pm = RTC_AM;
        
        /* ��Ӳ����ȡ��ǰʱ�� */
   //     rtc_current_time_get(&rtc_initpara);

        if(ERROR == rtc_init(&rtc_initpara)) {
            OLED_Printf(0,0,16,"RTC config failed!\n");
			OLED_Refresh();
        } else {
            rtc_show_time();
            RTC_BKP0 = BKP_VALUE; // ����������
        }
       if(ERROR == rtc_init(&rtc_initpara)){
			printf("\n\r** RTC time configuration failed! **\n\r");
		}else{
			printf("\n\r** RTC time configuration success! **\n\r");
			//rtc_show_time();
			RTC_BKP0 = BKP_VALUE;
		}
}

uint8_t bcd_to_decimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}


// ������ֵ
void adjust_value(int8_t delta) {
    switch(current_state) {
        case ADJ_YEAR:
            rtc_initpara.year = dec2bcd(CLAMP(bcd_to_decimal(rtc_initpara.year) + delta, 0, 99));
            break;
            
        case ADJ_MONTH:
            rtc_initpara.month = dec2bcd(CYCLIC(bcd_to_decimal(rtc_initpara.month) + delta, 1, 12));
            break;
            
        case ADJ_DAY: {
            uint8_t max_day = get_max_day(2000 + bcd_to_decimal(rtc_initpara.year), bcd_to_decimal(rtc_initpara.month));
            rtc_initpara.date = dec2bcd(CLAMP(bcd_to_decimal(rtc_initpara.date) + delta, 1, max_day));
            break;
        }
            
        case ADJ_HOUR:
            rtc_initpara.hour = dec2bcd(CYCLIC(bcd_to_decimal(rtc_initpara.hour) + delta, 0, 23));
            break;
            
        case ADJ_MINUTE:
            rtc_initpara.minute =dec2bcd( CYCLIC(bcd_to_decimal(rtc_initpara.minute) + delta, 0, 59));
            break;
            
        case ADJ_SECOND:
            rtc_initpara.second = dec2bcd(CYCLIC(bcd_to_decimal(rtc_initpara.second) + delta, 0, 59));
            break;
            
        case ADJ_CONFIRM:
            apply_rtc_changes();
            break;
    }
}

// �������¼�
void handle_key_event(uint8_t key) {
    switch(key) {
        case 1: // �л�������
			 if(current_state < ADJ_CONFIRM) {
                current_state = (AdjustState)(current_state + 1);
                if(current_state == ADJ_CONFIRM) {
                    //apply_rtc_changes(); // ����ȷ��״̬ʱ��������
                }
            } else {
                current_state = ADJ_YEAR; // ������ú�ص���ʼ״̬
            }
            Key_num=0;
            break;
		
			case 2: // ��ֵ����
            adjust_value(1);
			Key_num=0;
            break;
            
			case 3: // ��ֵ����
            adjust_value(-1);
			Key_num=0;
            break;
    }
}

//// ��ʾ��ǰ����״̬
//void show_adjust_status(void) {
//    const char *items[] = {"Year","Month","Day","Hour","Minute","Second","Confirm"};
//    OLED_Printf(0,0,16,"\n[%s] 20%02d-%02d-%02d %02d:%02d:%02d\n", 
//          items[current_state],
//          rtc_initpara.year, rtc_initpara.month, rtc_initpara.date,
//          rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
//}

// ��ʾ��ǰ����״̬
void show_adjust_status(void) {
	
    OLED_Printf(0,0,12," 20%0.2x-%0.2x-%0.2x %0.2x:%0.2x:%0.2x\n", 
          rtc_initpara.year, rtc_initpara.month, rtc_initpara.date,
          rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
	OLED_Printf(0,12,12,"%d",current_state);
	OLED_Refresh();
}



