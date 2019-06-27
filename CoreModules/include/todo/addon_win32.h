#pragma once

#include "../shared_in.h"

enum wnd_stile
{
	save_def = 0,
	set_def,
	set_normal,
	set_fullscr_borderless,
};

inline void Win32_SetWindowFrame(Window* my_window, wnd_stile stile)
{
	static LONG UsedStile;
	static LONG NormalStyle;
	static LONG Borderless;
	static INT2 StartPos;
	static INT2 StartSize;

	static int last_op = -1;

	MakeOnce
	{
 		UsedStile	= GetWindowLong(my_window->GetWindowId().hwnd, GWL_STYLE);
		NormalStyle = WS_OVERLAPPEDWINDOW;
		Borderless	= WS_POPUP;
		StartPos	= { my_window->GetX(),	 my_window->GetY() };
		StartSize	= { my_window->GetWidth(), my_window->GetHeight() };
 	}

	if (stile == set_def)
	{
		my_window->SetTopMost(false);

		SetWindowLong(my_window->GetWindowId().hwnd, GWL_STYLE, UsedStile);
		SetWindowPos(my_window->GetWindowId().hwnd, NULL,
			StartPos.x, StartPos.y, StartSize.x, StartSize.y, SWP_FRAMECHANGED | (last_op == set_fullscr_borderless ? 0 : SWP_NOMOVE | SWP_NOSIZE) | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}
	
	if (stile == set_normal)
	{
		my_window->SetTopMost(false);

		SetWindowLong(my_window->GetWindowId().hwnd, GWL_STYLE, UsedStile | NormalStyle);
		SetWindowPos(my_window->GetWindowId().hwnd, NULL, StartPos.x, StartPos.y, StartSize.x, StartSize.y, SWP_FRAMECHANGED | (last_op == set_fullscr_borderless ? 0 : SWP_NOMOVE | SWP_NOSIZE) | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}
	
	if (stile == set_fullscr_borderless)
	{
		my_window->SetTopMost(true);

		StartPos = { my_window->GetX(),	 my_window->GetY() };
		StartSize = { my_window->GetWidth(), my_window->GetHeight() };
		int w = os_api::get_system_metric(os::os_sys_metric::MAIN_MONITOR_W).GetValue();
		int h = os_api::get_system_metric(os::os_sys_metric::MAIN_MONITOR_H).GetValue();

		SetWindowLong(my_window->GetWindowId().hwnd, GWL_STYLE, UsedStile & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU));
		SetWindowPos(my_window->GetWindowId().hwnd, HWND_TOP, 0, 0, w, h-2, SWP_FRAMECHANGED);
	}
	
	last_op = stile;
}