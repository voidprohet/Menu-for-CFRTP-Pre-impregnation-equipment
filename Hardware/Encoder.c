#include "stm32f10x.h"                  // Device header


//void Encoder_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//		
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;		//ARR
//	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		//PSC
//	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
//	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
//	
//	TIM_ICInitTypeDef TIM_ICInitStructure;
//	TIM_ICStructInit(&TIM_ICInitStructure);
//	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
//	TIM_ICInitStructure.TIM_ICFilter = 0xF;
//	TIM_ICInit(TIM1, &TIM_ICInitStructure);
//	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
//	TIM_ICInitStructure.TIM_ICFilter = 0xF;
//	TIM_ICInit(TIM1, &TIM_ICInitStructure);
//	
//	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
//	
//	TIM_Cmd(TIM1, ENABLE);
//	
//	TIM_SetCounter(TIM1, 2);
//}
void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1; // 修改为中央对齐模式1
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);

	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	TIM_Cmd(TIM1, ENABLE);

	TIM_SetCounter(TIM1, 2);
}


//int16_t Encoder_Get(void)
//{
//	int16_t temp = TIM_GetCounter(TIM1);
//	TIM_SetCounter(TIM1, 0);
//	return temp;
//}

int16_t Encoder_Get(void)
{
	int16_t temp = TIM_GetCounter(TIM1);
	int16_t direction = 1;

	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_RESET)
	{
		direction = -1;
	}

	TIM_SetCounter(TIM1, 0);

	return temp * direction;
}


int8_t d0;
int16_t Encoder_Get_Div4(void)
{
	/*除以4输出*/	
	if(TIM_GetCounter(TIM1)/4 - d0)
	{
		int16_t temp = (TIM_GetCounter(TIM1)/4 - d0);
		
		d0 = TIM_GetCounter(TIM1)/4;
		return temp;
	}
	return 0;
	
}
int8_t daytemp = 1;
int16_t Encoder_Get_Pow(void)	//指数
{
	
	if(daytemp > 0){daytemp--; return 0;}
	
	daytemp = 2;
	int16_t P = Encoder_Get_Div4();
	return P*P*P;
}

void Encoder_Reset(void)
{
	TIM_SetCounter(TIM1, 2);
}

