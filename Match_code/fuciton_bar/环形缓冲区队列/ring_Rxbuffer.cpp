/*
* @fuction:串口接收环形缓冲区的fifo实现
* @author:Xu Wang
*/
#include "stdio.h"
#include "ring_Rxbuffer.h"
/*
* @parm:一个基于Ququeue的结构体变量指针
* @fuction:环形队列缓冲区初始化
* @author:Xu Wang
*/
void Ququeue_init(Ququeue*q){
q->front=0;
q->rear=0;
for(int i=0;i<buffersize;i++){
q->ring_RxBuffer[i]=0;
}
}

/*
* @parm:一个基于Ququeue的结构体变量指针
* @fuction:判断环形缓冲区序列是否为满
* @author:Xu Wang
*/

bool Ququeue_is_full(Ququeue*q){
if(((q->rear+1)%buffersize)==q->front){
    return 1;
}
else{
    return 0;
}
}
/*
* @parm:一个基于Ququeue的结构体变量指针
* @fuction:判断环形缓冲区序列是否为空
* @author:Xu Wang
*/

bool Ququeue_is_empty(Ququeue*q){
if(q->rear==q->front){
return 1;
}
else{
return 0;
}
}

bool Ququeue_Rxbuffer_write(Ququeue *q,ElemType Txdata){
if(Ququeue_is_full(q)==1){
    return 0;
}
q->ring_RxBuffer[q->rear]=Txdata;//对首指针相当于写指针来接入对应数据
q->rear=(q->rear+1)%buffersize;
return true;
}

bool Ququeue_Rxbuffer_read(Ququeue *q,ElemType *Rxdata){
if(Ququeue_is_empty(q)==1){
    return 0;
}
*Rxdata=q->ring_RxBuffer[q->front];
q->front=(q->front+1)%buffersize;
return 1;
}