#ifndef __TIMER_H
#define __TIMER_H

void Timer_Init(void);

uint8_t Timer_GetFPS(void);				//获取帧率
void Timer_SetFreqCounter(void);		//放置计数器
uint8_t Timer_GetFreqCounter(void);		//获取频率

#endif
