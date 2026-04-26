#ifndef __KEY_H
#define __KEY_H

void Key_Init(void);

int8_t Key_Enter_Get(void);
int8_t Key_Back_Get(void);
int8_t Key_Up_Get(void);
int8_t Key_Down_Get(void);

void Key_Reset_All(void);	//清除所有按键标志

#endif
