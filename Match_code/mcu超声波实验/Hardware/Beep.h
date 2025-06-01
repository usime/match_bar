#ifndef __Beep_H
#define __Beep_H
#include "stm32f10x.h"
#include "sys.h"
void Beep_Init();
void Set_Beep_compare(int Compare);
int Dist_tocompare(float dist);
#endif 
