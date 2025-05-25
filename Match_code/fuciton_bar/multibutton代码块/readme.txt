//完整的使用步骤
1.先申请一个按键结构
struct Button button1;
2.初始化按键对象，绑定按键的GPIO电平读取接口read_button_pin() ，后一个参数设置有效触发电平
button_init(&button1, read_button_pin, 0);
3.注册按键事件
button_attach(&button1, SINGLE_CLICK, Callback_SINGLE_CLICK_Handler);
button_attach(&button1, DOUBLE_CLICK, Callback_DOUBLE_Click_Handler);
...
4.启动按键
button_start(&button1);
5.设置一个5ms间隔的定时器循环调用后台处理函数
while(1) {
    ...
    if(timer_ticks == 5) {
        timer_ticks = 0;
        button_ticks();
    }
}