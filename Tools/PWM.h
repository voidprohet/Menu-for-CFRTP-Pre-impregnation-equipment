#ifndef __PWM_H
#define __PWM_H


extern uint8_t PA1,PA2;
void PWM_Init1(void);
void PWM_Init2(void);
void PWM_Init3(void);

void PWM_SetPrescaler1(uint16_t Prescaler1);
void PWM_SetPrescaler2(uint16_t Prescaler2);
void PWM_SetPrescaler3(uint16_t Prescaler3);

void PWM_SetARR1(uint16_t Autoreload);
void PWM_SetCompare1(uint16_t Compare1);
void PWM_SetARR2(uint16_t Autoreload);
void PWM_SetCompare2(uint16_t Compare2);
void PWM_SetARR3(uint16_t Autoreload);
void PWM_SetCompare3(uint16_t Compare3);

void StartMotors(void);

void Dir_Turn(void);
void Dir_Init(void);

void PWM_Enable(void);
//void PWM_PWM1(void);
//void PWM_Timing(void);

void Toggle_PWM_Compare_Mode(uint16_t PWM1orTiming);     //0表示切换为冻结模式,1表示切换到PWM1
#endif
