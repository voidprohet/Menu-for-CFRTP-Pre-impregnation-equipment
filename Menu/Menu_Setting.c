#include "Menu.h"

struct Option_Class Menu_SettingOptionList[];

void Set_CursorStyle(void)
{
	if (Menu_Global.CursorStyle == reverse)
	{
		Menu_Global.CursorStyle = mouse;
		Menu_SettingOptionList[1].String = "光标风格[鼠标]";
	}
	else if (Menu_Global.CursorStyle == mouse)
	{
		Menu_Global.CursorStyle = frame;
		Menu_SettingOptionList[1].String = "光标风格[矩形]";
	}
	else
	{
		Menu_Global.CursorStyle = reverse;
		Menu_SettingOptionList[1].String = "光标风格[反相]";
	}
}

void Set_animation_speed(void)
{
	static int8_t State = 0;

	if (State == 0)
	{
		Menu_Global.Cursor_ActSpeed = 0.2;
		Menu_Global.Slide_ActSpeed = 4;
		Menu_SettingOptionList[2].String = "动画速度[快]";
		State++;
	}
	else if (State == 1)
	{
		Menu_Global.Cursor_ActSpeed = 1;
		Menu_Global.Slide_ActSpeed = 16;
		Menu_SettingOptionList[2].String = "动画速度[关]";
		State++;
	}
	else if (State == 2)
	{
		Menu_Global.Cursor_ActSpeed = 0.1;
		Menu_Global.Slide_ActSpeed = 1.26;
		Menu_SettingOptionList[2].String = "动画速度[慢]";
		State++;
	}
	else
	{
		Menu_Global.Cursor_ActSpeed = 0.15;
		Menu_Global.Slide_ActSpeed = 2;
		Menu_SettingOptionList[2].String = "动画速度[中]";
		State = 0;
	}
}

void Menu_RunSettingMenu(void)
{
	Menu_RunMenu(Menu_SettingOptionList);
}

struct Option_Class Menu_SettingOptionList[] = {
	{"<<<"},
	{"光标风格[鼠标]", Set_CursorStyle},
	{"动画速度[中]", Set_animation_speed},
	{".."}
};
