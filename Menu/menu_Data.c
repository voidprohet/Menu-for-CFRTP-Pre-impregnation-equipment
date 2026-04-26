#include "Menu.h"
#include "PWM.h"
#include "stdio.h"
#include "OLED.h"
#include <stdio.h>
#include <math.h>

extern const float A0, B0, C0;


void KunKun(void)
{
//	Motors_Init();
//	Timer_Init();
	Timer_Init();
	OLED_Init();
	static uint8_t count1 = 0,count2 = 0;
	while (count2<=100)
	{
		// Delay_ms(1000);
		OLED_Clear();

		//Menu_ShowWallpaper(Win11Wallpaper);
		
		//Menu_Showkunkun();
		OLED_ShowImage(0,0,64, 64, kunkun[count1++/10]);
		OLED_ShowImage(64,0,64, 64, kunkun[count1++/10]);
		count1 %= 130,count2++;
//		if (Menu_BackEvent()|Menu_EnterEvent())
//		break;
//	 Delay_ms(1000);
		OLED_Update();
	}
	count1 = 0,count2 = 0;
}
	

float NumPower = 0.01;//旋转编码器旋钮一次修改值

void Setting_SetCat1(void)
{
	Setting_Set(1);
}
void Setting_SetCat2(void)
{
	Setting_Set(2);
}
void Setting_SetCat3(void)
{
	Setting_Set(3);
}


//int8_t IsV1change=0,IsV2change=0,IsV3change=0,fakenter=0;

extern struct 
Motors_Class{float V1;float V2;float V3;float MotorA;float MotorB;float MotorC;} 
Motors;

int MotorToARR(float MotorsABC) 
{
    float result = (1000/MotorsABC);
    float rounded = round(result); // 四舍五入到最接近的整数
    int final_result = (int)rounded-1; // 转换为整数

//    printf("结果四舍五入到个位数: %d\n", final_result);

    return final_result;
}

int MotorToCRR(float MotorsABC) 
{
    float result = (500/MotorsABC);
    float rounded = round(result); // 四舍五入到最接近的整数
    int final_result = (int)rounded; // 转换为整数

//    printf("结果四舍五入到个位数: %d\n", final_result);

    return final_result;
}

//初始化电机的数据
void Tools_PWM_OutputUpdate(void);
extern float PWM_Output_PSC;
extern float PWM_Output_CCR;
extern float PWM_Output_ARR;
extern float PWM_Output_Freq;
extern float PWM_Output_Duty; // percent*100

void StartMotors(void)
{
	PWM_Init1();				//PWM初始化		PWM频率Freq = 72M / (PSC + 1) / 100		//PWM占空比Duty = CCR / 100
	PWM_SetPrescaler1(MotorToARR(Motors.MotorA));//
//	PWM_SetARR1(MotorToARR(Motors.MotorA));
//	PWM_SetCompare1((MotorToARR(Motors.MotorA)+1)/2);
	PWM_SetCompare1(50);
	PWM_Init2();//PWM初始化	
	PWM_SetPrescaler2(MotorToARR(Motors.MotorB));			
//	PWM_SetARR2(MotorToARR(Motors.MotorA));
//	PWM_SetCompare2((MotorToARR(Motors.MotorA)+1)/2);	
	PWM_SetCompare2(50);
	PWM_Init3();//PWM初始化	
	PWM_SetPrescaler3(MotorToARR(Motors.MotorC));			 
//	PWM_SetARR3(MotorToARR(Motors.MotorA));
//	PWM_SetCompare3((MotorToARR(Motors.MotorA)+1)/2);
	PWM_SetCompare3(50);
}
void StopMotors(void)
{
	PWM_Init1();//PWM初始化					//PWM占空比Duty = CCR / 100
	PWM_Init2();//PWM初始化	
	PWM_Init3();//PWM初始化	
}

extern struct Option_Class Menu_SettingOptionList[];

static struct Option_Class Menu_StartOptionList[] = 
{
	{"<<<"},
	{"开始",Menu_StartorStopMenu},		//开始暂停
	{"设置",Menu_SettingMenu},
	{"初始化",ALLMotorsPWM_Init},	   // 工具
//	{"坤坤",KunKun},  // 设置
	{"光标", Menu_RunSettingMenu}, 
	{"其他", Menu_Information}, // 信息
	{".."}
};

void Menu_SettingMenu(void)
{
	
	struct Option_Class SettingList[] = {
	{"<<<"},
	{"A %.2f r/min",settingv1,&Motors.V1},
	{"B %.2f r/min",settingv2,&Motors.V2},
    {"C %.2f  r/min",settingv3,&Motors.V3},
	{"保存更改",Menu_SaveMenu},	//参数设置
//	{"Save B=%.2f",Menu_SaveMenu,&Motors.MotorB},
//	{"Save C=%.2f",Menu_SaveMenu,&Motors.MotorC},
	{".."},};
	Menu_RunMenu(SettingList);
}

void Setting_Set(int8_t Catch)
{
	while (1)
	{
		OLED_Clear();

		switch (Catch) // 抓住的选项
		{
		case 1:
			Motors.V1 += Menu_RollEvent() * NumPower; 
			OLED_Printf(2, 8, 8, "A %.2f r/min", Motors.V1);
			break;
		case 2:
			Motors.V2 += Menu_RollEvent() * NumPower; 
			OLED_Printf(2, 24, 8, "B %.2f r/min", Motors.V2);
			break;
		case 3:
			Motors.V3 += Menu_RollEvent() * NumPower; //*的倍率为修改后倍率*旋钮
			OLED_Printf(2, 40, 8, "C %.2f  r/min", Motors.V3);
			break;
		default:
			break;
		}
		OLED_Update();
		if (Menu_BackEvent()|Menu_EnterEvent())
		{
			return;
		}
	}
}

void ALLMotorsPWM_Init(void)
{
//	Toggle_PWM_Compare_Mode(0);
	Motors.V1=A0;
	Motors.V2=B0;
	Motors.V3=C0;
	Motors.MotorA=A0;
	Motors.MotorB=B0;
	Motors.MotorC=C0;
	PWM_SetARR1(MotorToARR(Motors.MotorA));
	PWM_SetCompare1(MotorToCRR(Motors.MotorA));
	PWM_SetARR2(MotorToARR(Motors.MotorB));
	PWM_SetCompare2(MotorToCRR(Motors.MotorB));
	PWM_SetARR3(MotorToARR(Motors.MotorC));
	PWM_SetCompare3(MotorToCRR(Motors.MotorC));
//	Toggle_PWM_Compare_Mode(1);
//	KunKun();
}

void settingv1(void)
{		
	Setting_Set(1);
}

void settingv2(void)
{		
	Setting_Set(2);
}

void settingv3(void)
{		
	Setting_Set(3);
}

void Menu_RunMainMenu(void)
{
	Toggle_PWM_Compare_Mode(0);
	SaveToMotors();
	Toggle_PWM_Compare_Mode(0);
	Menu_RunMenu(Menu_StartOptionList);
}

void Menu_StartorStopMenu(void)
{
	if(strcmp(Menu_StartOptionList[1].String, "开始") == 0)
	{
		Menu_StartOptionList[1].String = "暂停";
		Toggle_PWM_Compare_Mode(1);//切换到PWM1模式
//		KunKun();
		
	}
	else if(strcmp(Menu_StartOptionList[1].String, "暂停") == 0)
	{
		Menu_StartOptionList[1].String = "开始";
		Toggle_PWM_Compare_Mode(0);//切换到冻结输出比较模式(TIMING模式)
	}
}

void Menu_SaveMenu(void)
{
	if(strcmp(Menu_StartOptionList[1].String, "开始") == 0)
	{
		Menu_StartOptionList[1].String = "暂停";
	}
	Toggle_PWM_Compare_Mode(0);
	Motors.MotorA= Motors.V1;
	Motors.MotorB= Motors.V2;
	Motors.MotorC= Motors.V3;//	Motors.Fulse = Motors.fulse 
	SaveToMotors();
	Toggle_PWM_Compare_Mode(1);	
//	StartMotors();
//	return;
	
//	KunKun();
	//启动函数中有PWM初始化函数，初始化函数会使用Motors.a.b.c
//	OLED_Printf(2, 36, 8, "V1=%.2f", Motors.V1);
//	OLED_Printf(2, 48, 8, "VA=%.2f", Motors.MotorA);
}

void SaveToMotors(void)
	{	
		
		PWM_SetARR1(MotorToARR(Motors.MotorA));
		PWM_SetCompare1(MotorToCRR(Motors.MotorA));
		PWM_SetARR2(MotorToARR(Motors.MotorB));
		PWM_SetCompare2(MotorToCRR(Motors.MotorB));
		PWM_SetARR3(MotorToARR(Motors.MotorC));
		PWM_SetCompare3(MotorToCRR(Motors.MotorC));
	}
	
void Menu_RunGamesMenu(void)
{
	static struct Option_Class Menu_GamesOptionList[] = {
		{"<<<"},
		{"Snake", }, 
		{"康威生命游戏", }, // 康威生命游戏,元胞自动机
		{".."}};

	Menu_RunMenu(Menu_GamesOptionList);
}

void Menu_Information(void)
{
	uint8_t Angle = 233;
	while (1)
	{
		OLED_Clear();

		OLED_ShowImage(88, 8, 32, 32, goutou);
		OLED_Rotation_Block(88 + 16, 8 + 16, 16, Angle * 360 / 256);

		Angle += Menu_RollEvent() * 8;
		Angle += 2;

		OLED_ShowString(0,  0, "制丝机二号"   , OLED_8X16);
		OLED_ShowString(0, 16, "杨磊团队" , OLED_8X16);
		OLED_ShowString(0, 32, "郝嘉明 姜启帆"    , OLED_8X16);
		OLED_ShowString(0, 48, "武汉理工大学", OLED_8X16);

		OLED_Update();
		if (Menu_EnterEvent())
		{
			return;
		}
		if (Menu_BackEvent())
		{
			return;
		}
	}
}

/**********************************************************/
