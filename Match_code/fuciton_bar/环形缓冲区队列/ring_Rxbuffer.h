#ifndef __ring_Rxbuffer__H
#define __ring_Rxbuffer__H
#define ElemType int
#define buffersize 2048
typedef struct{
ElemType ring_RxBuffer[buffersize];
int front;
int rear;
int byte_count;
}Ququeue;
void Ququeue_init(Ququeue*q);
bool Ququeue_is_full(Ququeue*q);
bool Ququeue_is_empty(Ququeue*q);
bool Ququeue_Rxbuffer_write(Ququeue *q,ElemType Txdata);
bool Ququeue_Rxbuffer_read(Ququeue *q,ElemType *Rxdata);
#endif