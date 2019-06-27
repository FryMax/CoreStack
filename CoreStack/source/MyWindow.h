#pragma once

#include "in.h"

// using inpit
// using window
// using imgui

static MouseControl	   Mouse;
static KeyboardControl Keyboard;

class MyInputReader : public InputHandler
{
public:
	inline void MouseMove    (const MouseEvent& e)      { Mouse.InputMove(e);     };
	inline void MouseWheel   (const MouseWheelEvent& e) { Mouse.InputWeel(e);     };
	inline void MousePress   (const MouseClickEvent& e) { Mouse.InputPress(e);    };
	inline void MouseRelease (const MouseClickEvent& e) { Mouse.InputPress(e);    };
	inline void KeyPress     (const KeyEvent& e)        { Keyboard.InputPress(e); };
	inline void KeyReleas    (const KeyEvent& e)        { Keyboard.InputPress(e); };

	//
	//  Win32 impl
	//
	inline bool ExInputHandler(const os_window_event_sms& sms)
	{
		auto msg    = sms.msg;
		auto lParam = sms.lp;
		auto wParam = sms.wp;

		if (ImGui::GetCurrentContext() == NULL)
			return false;

		ImGuiIO& io = ImGui::GetIO();

		io.KeyCtrl  = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
		io.KeyShift = (::GetKeyState(VK_SHIFT)   & 0x8000) != 0;
		io.KeyAlt   = (::GetKeyState(VK_MENU)    & 0x8000) != 0;
		io.KeySuper = false;

		::Keyboard.Switchers.KeyAlt   = io.KeyAlt;
		::Keyboard.Switchers.KeyCtrl  = io.KeyCtrl;
		::Keyboard.Switchers.KeyShift = io.KeyShift;
		::Keyboard.Switchers.KeySyper = ::GetKeyState(VK_LWIN) & 0x8000 != 0
			                         || ::GetKeyState(VK_RWIN) & 0x8000 != 0;
		switch (msg)
		{
		case WM_MOUSEMOVE:
		{
			io.MousePos.x = (signed short)(lParam);
			io.MousePos.y = (signed short)(lParam >> 16);
			return true;
		}
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		{
			int button = 0;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;

			io.MouseDown[button] = true;

			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
				::SetCapture(this->my_window.hwnd);

			return true;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			int button = 0;
			if (msg == WM_LBUTTONUP) button = 0;
			if (msg == WM_RBUTTONUP) button = 1;
			if (msg == WM_MBUTTONUP) button = 2;

			io.MouseDown[button] = false;

			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == this->my_window.hwnd)
				::ReleaseCapture();

			return true;
		}
		case WM_MOUSEWHEEL:
			
			io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			return true;

		#if defined(WM_MOUSEHWHEEL)
		
		case WM_MOUSEHWHEEL:
			
			io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			return true;
		
		#endif

		case WM_KEYDOWN: case WM_SYSKEYDOWN:

			if (wParam < 256)
				io.KeysDown[wParam] = 1;
			return true;

		case WM_KEYUP: case WM_SYSKEYUP:

			if (wParam < 256)
				io.KeysDown[wParam] = 0;
			return true;

		case WM_CHAR:

			if (wParam > 0 && wParam < 0x10000)
				io.AddInputCharacter((unsigned short)wParam);
			return true;

		case WM_SETCURSOR:

			//todo mb chech if cursor stile not changed
			if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
				return true;
			else
				return false;
		}

		return false;
	}
};