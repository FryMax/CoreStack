#pragma once
#include "pch.h"

// used os::inpit
// used os::window
// used imgui + impl

extern MouseState    Mouse;
extern KeyboardState Keyboard;

class MyInputReader : public interfaceInputHandler
{
public:
	void MouseMove    (const MouseEvent& e)      override { Mouse.InputMove(e);     };
	void MouseWheel   (const MouseWheelEvent& e) override { Mouse.InputWeel(e);     };
	void MousePress   (const MouseClickEvent& e) override { Mouse.InputPress(e);    };
	void MouseRelease (const MouseClickEvent& e) override { Mouse.InputPress(e);    };
	void KeyPress     (const KeyEvent& e)        override { Keyboard.InputPress(e); };
	void KeyReleas    (const KeyEvent& e)        override { Keyboard.InputPress(e); };

	// -------------------
	// Win32 platform code
	// -------------------
	EV_STATUS ExInputHandler(const os_window_event_msg& ev) override
	{
		auto msg    = ev.msg;
		auto lParam = ev.lp;
		auto wParam = ev.wp;

		if (ImGui::GetCurrentContext() == NULL)
			return EV_NOT_PROCESSED;

		ImGuiIO& io = ImGui::GetIO();

		io.KeyCtrl  = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
		io.KeyShift = (::GetKeyState(VK_SHIFT  ) & 0x8000) != 0;
		io.KeyAlt   = (::GetKeyState(VK_MENU   ) & 0x8000) != 0;
		io.KeySuper = (::GetKeyState(VK_LWIN   ) & 0x8000) != 0 || (::GetKeyState(VK_RWIN) & 0x8000) != 0;

		Keyboard.Switchers.KeyAlt   = io.KeyAlt;
		Keyboard.Switchers.KeyCtrl  = io.KeyCtrl;
		Keyboard.Switchers.KeyShift = io.KeyShift;
		Keyboard.Switchers.KeySyper = io.KeySuper;

		switch (msg)
		{
		case WM_MOUSEMOVE:

			io.MousePos.x = (signed short)(lParam);
			io.MousePos.y = (signed short)(lParam >> 16);

			return EV_PROCESSED;

		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		{
			int button = 0;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;

			io.MouseDown[button] = true;


			//TODO
			//if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
			//	::SetCapture(this->my_window.hwnd);

            return EV_PROCESSED;
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

			//TODO
			//if (!ImGui::IsAnyMouseDown() && ::GetCapture() == this->my_window.hwnd)
			//	::ReleaseCapture();

            return EV_PROCESSED;
		}
		case WM_MOUSEWHEEL:
			io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			return EV_PROCESSED;

		case WM_KEYDOWN: case WM_SYSKEYDOWN:

			if (wParam < 256)
				io.KeysDown[wParam] = 1;

			return EV_PROCESSED;

		case WM_KEYUP: case WM_SYSKEYUP:

			if (wParam < 256)
				io.KeysDown[wParam] = 0;

			return EV_PROCESSED;

		case WM_CHAR:

			if (wParam > 0 && wParam < 0x10000)
				io.AddInputCharacter((unsigned short)wParam);

			return EV_PROCESSED;

		case WM_SETCURSOR:

			//todo mb chech if cursor stile not changed
			if (LOWORD(lParam) == HTCLIENT && ImGui_Platform_UpdateMouseCursor())
				return EV_PROCESSED;
			else
				return EV_NOT_PROCESSED;

		}

		return EV_NOT_PROCESSED;
	}
};