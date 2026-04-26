#include "Tools.h"

struct Option_Class PWM_Output_Option_List[];

float PWM_Output_PSC;
float PWM_Output_CCR;
float PWM_Output_ARR;
float PWM_Output_Freq;
float PWM_Output_Duty; // percent*100

float NumPower = 1;

void Tools_PWM_Output_SetFreq(float SetFreq) // 设置输出频率
{
	if (SetFreq > 10000)
	{
		SetFreq = 10000;
	}
	if (SetFreq < 1)
	{
		SetFreq = 1;
	}
	PWM_Output_PSC = ((float)72000000 / (PWM_Output_ARR + 1)) / (SetFreq)-1;
	if (PWM_Output_PSC < 0)
	{
		PWM_Output_PSC = 0;
	}
	PWM_Output_Freq = ((float)72000000 / (PWM_Output_ARR + 1)) / (PWM_Output_PSC + 1);
	PWM_SetPrescaler(PWM_Output_PSC);
}

void Tools_PWM_Output_SetDuty(float SetDuty) // 设置输出空占比
{
	if (SetDuty < 0)
	{
		SetDuty = 0;
	}
	if (SetDuty > 100)
	{
		SetDuty = 100;
	}
	PWM_Output_CCR = (SetDuty / 100) * (PWM_Output_ARR + 1);
	PWM_Output_Duty = (float)PWM_Output_CCR / (PWM_Output_ARR + 1) * 100;
	PWM_SetCompare1(PWM_Output_CCR);
}

void Tools_PWM_Output_SetPSC(float PSC) //
{
	PWM_Output_PSC = PSC;
	PWM_Output_Freq = ((float)72000000 / (PWM_Output_ARR + 1)) / (PWM_Output_PSC + 1);
	PWM_SetPrescaler(PWM_Output_PSC);
}
void Tools_PWM_Output_SetCCR(float CCR) //
{
	PWM_Output_CCR = CCR;
	PWM_Output_Duty = (float)PWM_Output_CCR / (PWM_Output_ARR + 1) * 100;
	PWM_SetCompare1(PWM_Output_CCR);
}
void Tools_PWM_Output_SetARR(float ARR) //
{
	PWM_Output_ARR = ARR;
	PWM_Output_Freq = ((float)72000000 / (PWM_Output_ARR + 1)) / (PWM_Output_PSC + 1);
	PWM_Output_Duty = (float)PWM_Output_CCR / (PWM_Output_ARR + 1) * 100;
	PWM_SetPeriod(PWM_Output_ARR);
}

void Tools_PWM_OutputUpdate(void) // 更新输出频率及占空比数值
{
	PWM_Output_Freq = ((float)72000000 / (PWM_Output_ARR + 1)) / (PWM_Output_PSC + 1);
	PWM_Output_Duty = (float)PWM_Output_CCR / (PWM_Output_ARR + 1) * 100;
	PWM_SetCompare1(PWM_Output_CCR);
	PWM_SetPrescaler(PWM_Output_PSC);
	PWM_SetPeriod(PWM_Output_ARR);
}

void Tools_PWM_Output_Set(int8_t Catch)
{
	while (1)
	{
		OLED_Clear();

		switch (Catch) // 抓住的选项
		{
		case 1:
			PWM_Output_Freq += Menu_RollEvent() * NumPower; // 频率加上 编码器偏移量乘以位权
			Tools_PWM_Output_SetFreq(PWM_Output_Freq);		// 设置频率
			OLED_Printf(2, 24, 8, "Freq = (%.1f)", PWM_Output_Freq);
			break;
		case 2:
			PWM_Output_Duty += Menu_RollEvent() * NumPower / 10;
			Tools_PWM_Output_SetDuty(PWM_Output_Duty); // 设置频率
			OLED_Printf(2, 24, 8, "Duty = (%.1f)", PWM_Output_Duty);
			break;
		case 3:
			PWM_Output_PSC += Menu_RollEvent() * NumPower;
			Tools_PWM_Output_SetPSC(PWM_Output_PSC);
			OLED_Printf(2, 24, 8, "PSC = (%.f)", PWM_Output_PSC);
			break;
		case 4:
			PWM_Output_CCR += Menu_RollEvent() * NumPower;
			Tools_PWM_Output_SetCCR(PWM_Output_CCR);
			OLED_Printf(2, 24, 8, "CCR = (%.f)", PWM_Output_CCR);
			break;
		case 5:
			PWM_Output_ARR += Menu_RollEvent() * NumPower;
			Tools_PWM_Output_SetARR(PWM_Output_ARR);
			OLED_Printf(2, 24, 8, "ARR = (%.f)", PWM_Output_ARR);
			break;
		default:
			break;
		}
		if (Menu_BackEvent())
		{
			Tools_PWM_Output_SetPSC((uint16_t)PWM_Output_PSC);
			Tools_PWM_Output_SetCCR((uint16_t)PWM_Output_CCR);
			NumPower = 1;
			return;
		}
		if (Menu_EnterEvent()) // 加乘方
		{
			NumPower *= 10; // 幂
			if (NumPower > 100)
			{
				NumPower = 1;
			}
		}
		OLED_Printf(0, 48, 8, "Power: x%.f", NumPower);
		OLED_Update();
	}
}

void Tools_PWM_Output_SetCat1(void)
{
	Tools_PWM_Output_Set(1);
}
void Tools_PWM_Output_SetCat2(void)
{
	Tools_PWM_Output_Set(2);
}
void Tools_PWM_Output_SetCat3(void)
{
	Tools_PWM_Output_Set(3);
}
void Tools_PWM_Output_SetCat4(void)
{
	Tools_PWM_Output_Set(4);
}
void Tools_PWM_Output_SetCat5(void)
{
	Tools_PWM_Output_Set(5);
}

struct Option_Class PWM_Output_Option_List[] = {
	{"<<<"},
	{"Freq:%.1f", Tools_PWM_Output_SetCat1, &PWM_Output_Freq},
	{"Duty:%.1f", Tools_PWM_Output_SetCat2, &PWM_Output_Duty},
	{"PSC: %.f", Tools_PWM_Output_SetCat3, &PWM_Output_PSC},
	{"CCR: %.f", Tools_PWM_Output_SetCat4, &PWM_Output_CCR},
	{"ARR: %.f", Tools_PWM_Output_SetCat5, &PWM_Output_ARR},
	{".."},
};


void Tools_PWM_Output(void)
{
//	static int8_t PWM_Output_isInit = 1;
//	if (PWM_Output_isInit)
	{
//		PWM_Output_isInit = 0;
		
		PWM_Init(); // PWM初始化
		// IC_Init();	// 输入捕获初始化

		PWM_Output_PSC = 71;
		PWM_Output_CCR = 2500;
		PWM_Output_ARR = 19999;
		Tools_PWM_OutputUpdate();
	}

	Menu_RunMenu(PWM_Output_Option_List);
}
