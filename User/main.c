#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "Timer.h"
#include "Encoder.h"
#include "Key.h"
#include "OLED.h"
#include "Menu.h"
#include "PWM.h"

int Power_Off(void);				 // 关机
const float A0=0.050, B0=1.00, C0=0.50;
//修改日志2024.10.9:
//故障记录：电机B的实际速度为显示速度的十倍；
//解决方法：将电机B的对应定时器的预分频值调为十倍，问题解决。
//制丝机中调整电机速度通过调整计数周期来实现，故该解决办法不干涉原有程序。

struct 
Motors_Class{float V1;float V2;float V3;float MotorA;float MotorB;float MotorC;} 
Motors={
	.MotorA=A0,
	.MotorB=B0,
	.MotorC=C0,
	.V1=A0,
	.V2=B0,
	.V3=C0};

void Motors_Init(void)
{
	Motors.MotorA=A0;
	Motors.MotorB=B0;
	Motors.MotorC=C0;
	Motors.V1=A0;
	Motors.V2=B0;
	Motors.V3=C0;
//	StartMotors();
}

uint8_t PA1=1,PA2=0;


void EXTI3_Config(void);
void TIM1_Config(void);
void NVIC_Config(void);
void delay_ms(uint32_t ms);

volatile uint8_t trigger_enabled = 1;


// 配置外部中断
//void EXTI3_Config(void)
//{
//    // 使能GPIOA时钟
//    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

//    // 配置PA3为浮空输入
//    GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
//    GPIOA->CRL |= GPIO_CRL_CNF3_1;

//    // 使能AFIO时钟
//    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

//    // 将PA3连接到EXTI线3
//    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI3;
//    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PA;

//    // 配置EXTI线3为上升沿触发
//    EXTI->FTSR &= ~EXTI_FTSR_TR3;
//    EXTI->RTSR |= EXTI_RTSR_TR3;

//    // 使能EXTI线3中断
//    EXTI->IMR |= EXTI_IMR_MR3;
//}
void EXTI3_Config(void)
{
    // 使能GPIOA时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // 配置PA3为下拉输入
    GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
    GPIOA->CRL |= GPIO_CRL_CNF3_0;

    // 使能AFIO时钟
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    // 将PA3连接到EXTI线3
    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI3;
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PA;

    // 配置EXTI线3为上升沿触发
    EXTI->FTSR &= ~EXTI_FTSR_TR3;
    EXTI->RTSR |= EXTI_RTSR_TR3;

    // 使能EXTI线3中断
    EXTI->IMR |= EXTI_IMR_MR3;
}

// 配置定时器1
void TIM1_Configuration(void)
{
    // 1. 使能 TIM1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // 2. 设置 TIM1 的预分频器和自动重载寄存器值
    TIM1->PSC = 7200-1; // 预分频，fCK_PSC / (PSC[15:0] + 1)，以1MHz为时钟
    TIM1->ARR = 30000-1; // 自动重载寄存器值，以达到你需要的中断间隔，即 10s （1MHz / 10000 = 100 Hz, 10000 counts = 100 Hz => 10s）

    // 3. 使能更新中断 和 计数器
    TIM1->DIER |= TIM_DIER_UIE; // 使能更新中断
    TIM1->CR1 |= TIM_CR1_CEN; // 启动计数器

    // 4. 配置 NVIC
    NVIC_EnableIRQ(TIM1_UP_IRQn); // 使能 TIM1 更新事件中断
    NVIC_SetPriority(TIM1_UP_IRQn, 1); // 可设置优先级根据需求
}

void NVIC_Config(void)
{
    // 配置外部中断优先级
    NVIC_SetPriority(EXTI3_IRQn, 0);
    NVIC_EnableIRQ(EXTI3_IRQn);

	// 配置定时器1优先级
	NVIC_EnableIRQ(TIM1_UP_IRQn);  // Enable TIM1 update event interrupt
	NVIC_SetPriority(TIM1_UP_IRQn, 1);

}

// 外部中断3中断处理函数
void EXTI3_IRQHandler(void)
{
	Delay_ms(15);
	
	// 确认外部中断线PA3已触发
	if ((GPIOA->IDR & GPIO_IDR_IDR3) == 0)
    {
        // 如果PA3不是高电平状态，立即退出中断
	    EXTI->IMR &= ~EXTI_IMR_MR3;// 关闭外部中断
		// 清除中断标志
		EXTI->PR |= EXTI_PR_PR3;
//		Dir_Init();//电机换向函数
		TIM1->SR &= ~TIM_SR_UIF;
//		Delay_ms(500);
		//TIM1->CR1 |= TIM_CR1_CEN;
		EXTI->IMR |= EXTI_IMR_MR3; // 使能外部中断
        return;
    }
	if (EXTI->PR & EXTI_PR_PR3)//判断中断是否触发
	{
		EXTI->IMR &= ~EXTI_IMR_MR3;// 关闭外部中断
		Dir_Init();//电机换向函数
		// 清除中断标志
		EXTI->PR |= EXTI_PR_PR3;
		TIM1->SR &= ~TIM_SR_UIF;
		//TIM1->CR1 |= TIM_CR1_CEN;
		EXTI->IMR |= EXTI_IMR_MR3; // 使能外部中断
		Delay_ms(800);
	}
}

// 5. 实现 TIM1 中断服务函数
void TIM1_UP_IRQHandler(void)
{
    if(TIM1->SR & TIM_SR_UIF) // 检查更新中断标志
    {
        TIM1->SR &= ~TIM_SR_UIF; // 清除更新中断标志

        

        // 关闭定时器1
        TIM1->CR1 &= ~TIM_CR1_CEN;
	    // 无需检查 trigger_enabled，直接重新启用外部中断
        EXTI->IMR |= EXTI_IMR_MR3; // 使能外部中断
    }
}

// 毫秒级延时函数
void delay_ms(uint32_t ms)
{
    volatile uint32_t nCount;
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    nCount = (RCC_Clocks.HCLK_Frequency / 10000) * ms;
    for (; nCount != 0; nCount--);
}


int main(void)
{
	Dir_Init();

	EXTI3_Config();

	// 初始化中断向量
	NVIC_Config();
	
	// 初始化定时器
	TIM1_Configuration();

	
	Timer_Init();
	OLED_Init();
	Encoder_Init();
	Key_Init();
	uint8_t count = 0;
	while (1)
	{
		// Delay_ms(1000);
		OLED_Clear();

		//Menu_ShowWallpaper(Win11Wallpaper);
		
		//Menu_Showkunkun();
		OLED_ShowImage(0,0,64, 64, kunkun[count++/10]);
		OLED_ShowImage(64,0,64, 64, kunkun[count++/10]);
		count %= 130;
		PWM_Enable();
		if (Menu_BackEvent()|Menu_EnterEvent())
		{
			Menu_RunMainMenu();
			//Power_Off();
		}
		OLED_Update();
	}
}
	



int Power_Off(void) // 关机
{
	/*关闭外设区*******/
	OLED_Clear();
	OLED_Update();
	/*******关闭外设区*/

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // Enables or disables the Low Speed APB (APB1) peripheral clock.
	PWR_WakeUpPinCmd(ENABLE);

	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI); // SIOP模式
	SystemInit();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE); // Enables or disables the Low Speed APB (APB1) peripheral clock.
	PWR_WakeUpPinCmd(DISABLE);

	Key_Reset_All();
	Key_Reset_All();

	return 1;
}
