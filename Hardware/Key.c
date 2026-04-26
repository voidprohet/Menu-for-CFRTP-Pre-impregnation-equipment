#include "stm32f10x.h"                  // Device header
#include "Key.h"

struct 
{
	uint8_t Enter;	//确认键
	uint8_t Back;	//返回键
	uint8_t Up;		//上
	uint8_t Down;	//下
} Key;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14 | EXTI_Line12 | EXTI_Line13 | EXTI_Line15;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	Key_Reset_All();
}

/********************************************/

int8_t Key_Enter_Get(void)	//确认键
{
	if(Key.Enter)
	{
		Key.Enter = 0;
		return 1;
	}
	return 0;
}

int8_t Key_Back_Get(void)	//返回键
{
	if(Key.Back)
	{
		Key.Back = 0;
		return 1;
	}
	return 0;
}

int8_t Key_Up_Get(void)
{
	if(Key.Up)
	{
		Key.Up = 0;
		return 1;
	}
	return 0;
}

int8_t Key_Down_Get(void)
{
	if(Key.Down)
	{
		Key.Down = 0;
		return 1;
	}
	return 0;
}

void Key_Reset_All(void)	//清除所有按键标志位
{
	Key.Enter = 0;
	Key.Back = 0;
	Key.Up = 0;
	Key.Down = 0;
}


void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line14) == SET) //确认键
	{
		int Delay = 200000; while(Delay--);
		
		Key.Enter = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
		
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	
	if (EXTI_GetITStatus(EXTI_Line12) == SET) //返回键
	{
		int Delay = 200000; while(Delay--);
	
		Key.Back = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
		
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
	
	if (EXTI_GetITStatus(EXTI_Line13) == SET) //返回键
	{
		int Delay = 200000; while(Delay--);

		Key.Down = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
		
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	
	if (EXTI_GetITStatus(EXTI_Line15) == SET) //返回键
	{
		int Delay = 200000; while(Delay--);

		Key.Up = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
		
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

