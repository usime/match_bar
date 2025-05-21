/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-16     armink       first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "Key.h"
#define LED_PIN	34
int Key_num=0;
#include <rtthread.h>
#include <rtdevice.h>
#include "Key.h"
#include "Usart.h"
#define LED_PIN 34

static rt_bool_t led_on = RT_FALSE;  // 记录LED状态

int main(void)
{
//    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);  // 初始化LED引脚
	
    uart_sample_init();
	
    while(1)
    {
		 uart_send_string("hello world\r\n");
        rt_thread_mdelay(1000);
////        int key_num = Key_scan();
////        
////        // 仅在按键按下时改变LED状态
////        if(key_num == 1)
////        {
////           
////            rt_pin_write(LED_PIN,  PIN_LOW);
////        }
////        else{
////		led();
////		
////		}
////        rt_thread_mdelay(10);  // 降低CPU占用率
    }
    
    return 0;
}



int led(void)
{
		rt_uint8_t count;
		rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);  
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    
    return 0;
}
MSH_CMD_EXPORT(Key_scan, RT-Thread first key sample);
MSH_CMD_EXPORT(led, RT-Thread first led sample);
MSH_CMD_EXPORT(uart_send_string, RT-Thread first led sample);
