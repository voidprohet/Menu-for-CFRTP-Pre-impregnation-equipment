#include "stm32f10x.h"                  // Device header
//extern struct Motor_Class Motors;	//调取主文件定义的储存电机数据的结构体
/**
  * 函    数：PWM初始化
  * 参    数：无
  * 返 回 值：无
  */
#include "Delay.h" 

extern uint8_t PA1,PA2;

void Dir_Turn(void)
{
	if(PA1==0)
	{	
		PA1=1,PA2=0;
	}
	else if(PA1!=0)
	{	
		PA1=0,PA2=1;
	}
}

void Dir_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
															//使用各个外设前必须开启时钟，否则对外设的操作无效
	
	/*GPIO初始化*/
	static GPIO_InitTypeDef GPIO_InitStructure1;					//定义结构体变量
	
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_1;				//GPIO引脚，赋值为第1号引脚
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
	
	GPIO_Init(GPIOA, &GPIO_InitStructure1);					//将赋值后的构体变量传递给GPIO_Init函数
															//函数内部会自动根据结构体的参数配置相应寄存器
															//实现GPIOA的初始化
	static GPIO_InitTypeDef GPIO_InitStructure2;					//定义结构体变量
	
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_2;				//GPIO引脚，赋值为第2号引脚
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
	
	GPIO_Init(GPIOA, &GPIO_InitStructure2);					//将赋值后的构体变量传递给GPIO_Init函数
															//函数内部会自动根据结构体的参数配置相应寄存器
	if (PA1  == 1)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);	
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		PA1=0,PA2=1;			//则设置PA1引脚为高电平
	}
	else if (PA1  == 0)													//否则，即当前引脚输出高电平
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_2);	
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
			PA1=1,PA2=0;			//则设置PA1引脚为低电平
	}														
}

  
void PWM_Init1(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);			//开启GPIOA的时钟
	
	/*GPIO重映射*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//开启AFIO的时钟，重映射必须先开启AFIO的时钟
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);			//将TIM2的引脚部分重映射，具体的映射方案需查看参考手册
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);		//将JTAG引脚失能，作为普通GPIO引脚使用
	
	/*GPIO初始化*/
	static GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//将PA0引脚初始化为复用推挽输出	
																	//受外设控制的引脚，均需要配置为复用模式		
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	static TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 12500-1;					//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 270-1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元
	
	/*输出比较初始化*/
	static TIM_OCInitTypeDef TIM_OCInitStructure;							//定义结构体变量
	TIM_OCStructInit(&TIM_OCInitStructure);							//结构体初始化，若结构体没有完整赋值
																	//则最好执行此函数，给结构体所有成员都赋一个默认值
																	//避免结构体初值不确定的问题
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;				//输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);						//将结构体变量交给TIM_OC1Init，配置TIM2的输出比较通道1
	
	/*TIM使能*/
	TIM_Cmd(TIM2, ENABLE);			//使能TIM2，定时器开始运行
}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare1(uint16_t Compare1)
{
	TIM_SetCompare1(TIM2, Compare1);		//设置CCR1的值
}

/**
  * 函    数：PWM设置PSC
  * 参    数：Prescaler 要写入的PSC的值，范围：0~65535
  * 返 回 值：无
  * 注意事项：PSC和ARR共同决定频率，此函数仅设置PSC的值，并不直接是频率
  *           频率Freq = CK_PSC / (PSC + 1) / (ARR + 1)
  */
void PWM_SetPrescaler1(uint16_t Prescaler1)
{
	TIM_PrescalerConfig(TIM2, Prescaler1, TIM_PSCReloadMode_Immediate);		//设置PSC的值
}

void PWM_SetARR1(uint16_t Autoreload)
{
	TIM_SetAutoreload(TIM2,Autoreload);								//设置ARR的值
}

void PWM_SetARR2(uint16_t Autoreload)
{
	TIM_SetAutoreload(TIM3,Autoreload);
}

void PWM_SetARR3(uint16_t Autoreload)
{
	TIM_SetAutoreload(TIM4,Autoreload);
}

//***************************************************************************************
void PWM_Init2(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);			//开启GPIOA的时钟
	
	/*GPIO重映射*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//开启AFIO的时钟，重映射必须先开启AFIO的时钟
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);			//将TIM2的引脚部分重映射，具体的映射方案需查看参考手册
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);		//将JTAG引脚失能，作为普通GPIO引脚使用
	
	/*GPIO初始化*/
	static GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		//GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//将PA0引脚初始化为复用推挽输出	
																	//受外设控制的引脚，均需要配置为复用模式		
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM3);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	static TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 5000-1;					//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 2700-1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元
	
	/*输出比较初始化*/
	static TIM_OCInitTypeDef TIM_OCInitStructure;							//定义结构体变量
	TIM_OCStructInit(&TIM_OCInitStructure);							//结构体初始化，若结构体没有完整赋值
																	//则最好执行此函数，给结构体所有成员都赋一个默认值
																	//避免结构体初值不确定的问题
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;				//输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);						//将结构体变量交给TIM_OC1Init，配置TIM2的输出比较通道1
	
	/*TIM使能*/
	TIM_Cmd(TIM3, ENABLE);			//使能TIM2，定时器开始运行
}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare2(uint16_t Compare2)
{
	TIM_SetCompare1(TIM3, Compare2);		//设置CCR1的值
}

/**
  * 函    数：PWM设置PSC
  * 参    数：Prescaler 要写入的PSC的值，范围：0~65535
  * 返 回 值：无
  * 注意事项：PSC和ARR共同决定频率，此函数仅设置PSC的值，并不直接是频率
  *           频率Freq = CK_PSC / (PSC + 1) / (ARR + 1)
  */
void PWM_SetPrescaler2(uint16_t Prescaler2)
{
	TIM_PrescalerConfig(TIM3, Prescaler2, TIM_PSCReloadMode_Immediate);		//设置PSC的值
	
}

///************************************************************************************************
void PWM_Init3(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//开启GPIOA的时钟
	
	/*GPIO重映射*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//开启AFIO的时钟，重映射必须先开启AFIO的时钟
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);			//将TIM2的引脚部分重映射，具体的映射方案需查看参考手册
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);		//将JTAG引脚失能，作为普通GPIO引脚使用
	
	/*GPIO初始化*/
	static GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		//GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//将PA0引脚初始化为复用推挽输出	
																	//受外设控制的引脚，均需要配置为复用模式		
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM4);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	static TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 2500-1 ;					//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 270-1	;		//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元
	
	/*输出比较初始化*/
	static TIM_OCInitTypeDef TIM_OCInitStructure;							//定义结构体变量
	TIM_OCStructInit(&TIM_OCInitStructure);							//结构体初始化，若结构体没有完整赋值
																	//则最好执行此函数，给结构体所有成员都赋一个默认值
																	//避免结构体初值不确定的问题
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;				//输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);						//将结构体变量交给TIM_OC1Init，配置TIM2的输出比较通道1
	
	/*TIM使能*/
	TIM_Cmd(TIM4, ENABLE);			//使能TIM2，定时器开始运行
}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare3(uint16_t Compare3)
{
	TIM_SetCompare1(TIM4, Compare3);		//设置CCR1的值
}

/**
  * 函    数：PWM设置PSC
  * 参    数：Prescaler 要写入的PSC的值，范围：0~65535
  * 返 回 值：无
  * 注意事项：PSC和ARR共同决定频率，此函数仅设置PSC的值，并不直接是频率
  *           频率Freq = CK_PSC / (PSC + 1) / (ARR + 1)
  */
void PWM_SetPrescaler3(uint16_t Prescaler3)
{
	TIM_PrescalerConfig(TIM4, Prescaler3, TIM_PSCReloadMode_Immediate);		//设置PSC的值
}

void PWM_Enable(void)
{
	PWM_Init1();
	PWM_SetCompare1((uint16_t)(12500/2));
	PWM_Init2();
	PWM_SetCompare2((uint16_t)(5000/2));
	PWM_Init3();
	PWM_SetCompare3((uint16_t)(2500/2));
}

//void PWM_PWM1(void)
//{
//	TIM_SelectOCxM(TIM2,TIM_Channel_1,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM2,TIM_Channel_2,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM2,TIM_Channel_3,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM2,TIM_Channel_4,TIM_OCMode_PWM1);
//	
//	TIM_SelectOCxM(TIM3,TIM_Channel_1,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM3,TIM_Channel_2,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM3,TIM_Channel_3,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM3,TIM_Channel_4,TIM_OCMode_PWM1);
//	
//	TIM_SelectOCxM(TIM4,TIM_Channel_1,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM4,TIM_Channel_2,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM4,TIM_Channel_3,TIM_OCMode_PWM1);
//	TIM_SelectOCxM(TIM4,TIM_Channel_4,TIM_OCMode_PWM1);
//	
//}

//void PWM_Timing(void)
//{
//	TIM_SelectOCxM(TIM2,TIM_Channel_1,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM3,TIM_Channel_1,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM4,TIM_Channel_1,TIM_OCMode_Timing);
//	
//	TIM_SelectOCxM(TIM2,TIM_Channel_2,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM3,TIM_Channel_2,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM4,TIM_Channel_2,TIM_OCMode_Timing);
//	
//	TIM_SelectOCxM(TIM2,TIM_Channel_3,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM3,TIM_Channel_3,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM4,TIM_Channel_3,TIM_OCMode_Timing);

//	TIM_SelectOCxM(TIM2,TIM_Channel_4,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM3,TIM_Channel_4,TIM_OCMode_Timing);
//	TIM_SelectOCxM(TIM4,TIM_Channel_4,TIM_OCMode_Timing);
//}

void Toggle_PWM_Compare_Mode(uint16_t PWM1orTiming)
{
	if(PWM1orTiming==0)
	{
			// 根据当前的PWM输出比较模式，切换到另一种模式
		if ((TIM2->CCMR1 &(TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2))== (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2))
		{	// 如果当前是PWM1模式，则切换到TIMING模式
			TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
			TIM2->CCMR1 |= TIM_CCMR1_OC1M_2; // 设置为TIMING模式
		}
			// 根据当前的PWM输出比较模式，切换到另一种模式
		if ((TIM3->CCMR1 & (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2))== (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2))
		{	// 如果当前是PWM1模式，则切换到TIMING模式
			TIM3->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
			TIM3->CCMR1 |= TIM_CCMR1_OC1M_2; // 设置为TIMING模式
		}
			// 根据当前的PWM输出比较模式，切换到另一种模式
		if ((TIM4->CCMR1 & (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2))== (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2))
		{	// 如果当前是PWM1模式，则切换到TIMING模式
			TIM4->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
			TIM4->CCMR1 |= TIM_CCMR1_OC1M_2; // 设置为TIMING模式
		}
        }
	if(PWM1orTiming==1)
	{
			// 根据当前的PWM输出比较模式，切换到另一种模式
		if (((TIM2->CCMR1 & TIM_CCMR1_OC1M_0) == 0)|((TIM2->CCMR1 & TIM_CCMR1_OC1M) == 0))
		{
			// 如果当前是TIMING模式，则切换到PWM1模式
			TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
			TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // 设置为PWM模式1
		}
//		else
//		{
//			// 如果当前是PWM1模式，则切换到TIMING模式
//			TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
//			TIM2->CCMR1 |= TIM_CCMR1_OC1M; // 设置为TIMING模式
//		}
//			// 根据当前的PWM输出比较模式，切换到另一种模式
		if (((TIM3->CCMR1 & TIM_CCMR1_OC1M_0) == 0)|((TIM3->CCMR1 & TIM_CCMR1_OC1M) == 0))
		{
			// 如果当前是TIMING模式，则切换到PWM1模式
			TIM3->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
			TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // 设置为PWM模式1
		}
//		else
//		{
//			// 如果当前是PWM1模式，则切换到TIMING模式
//			TIM3->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
//			TIM3->CCMR1 |= TIM_CCMR1_OC1M; // 设置为TIMING模式
//		}
//			// 根据当前的PWM输出比较模式，切换到另一种模式
		if (((TIM4->CCMR1 & TIM_CCMR1_OC1M_0) == 0)|((TIM4->CCMR1 & TIM_CCMR1_OC1M) == 0))
		{
			// 如果当前是TIMING模式，则切换到PWM1模式
			TIM4->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
			TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // 设置为PWM模式1
		}
//		else
//		{
//			// 如果当前是PWM1模式，则切换到TIMING模式
//			TIM4->CCMR1 &= ~TIM_CCMR1_OC1M; // 清除原先的模式
//			TIM4->CCMR1 |= TIM_CCMR1_OC1M; // 设置为TIMING模式
//		}
        }
}
