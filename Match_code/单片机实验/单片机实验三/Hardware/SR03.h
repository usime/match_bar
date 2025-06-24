#ifndef _SR03_H
#define _SR03_H
void SR03_Init(void);
void SR03_Strat(void);
int moving_filter(int Data, int filter_buffer[], int* buffer_index);
int median_filter(int Data, int median_buffer[], int* buffer_index);
float linear_compensation_function(float measured_distance);
#endif

