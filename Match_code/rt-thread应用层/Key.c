#include <rtthread.h>
#include <rtdevice.h>
#define Key_pin 36 //PA2

int Key_scan(void)
{
  static rt_uint8_t Key_flag=0;
	
    rt_pin_mode(Key_pin, 0x01);  
    
	if(rt_pin_read(Key_pin)==0){
		rt_thread_mdelay(20);
		while(rt_pin_read(Key_pin)==0);
		rt_thread_mdelay(20);
		Key_flag=1;
	}
	return Key_flag;   
}