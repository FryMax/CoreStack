#pragma once

#include "in.h"

static MouseState	   Mouse;
static KeyboardState Keyboard;

class MyInputReader : public interfaceInputHandler
{
public:
	void MouseMove   (const MouseEvent& e)      { Mouse.InputMove(e); };
	void MouseWheel  (const MouseWheelEvent& e) { Mouse.InputWeel(e); };
	void MousePress  (const MouseClickEvent& e) { Mouse.InputPress(e); };
	void MouseRelease(const MouseClickEvent& e) { Mouse.InputPress(e); };
	void KeyPress    (const KeyEvent& e)        { Keyboard.InputPress(e); };
	void KeyReleas   (const KeyEvent& e)        { Keyboard.InputPress(e); };

	//
	//  Win32 depended
	//
	bool ExInputHandler(const os_window_event_msg& sms)
	{
		SDL_Event event;
		std::memcpy(&event, &sms, sizeof(SDL_Event));

		if (ImGui::GetCurrentContext() == NULL)
			return false;

		ImGuiIO& io = ImGui::GetIO();
		switch (event.type)
		{
		case SDL_MOUSEMOTION:
		{
			int local_x = event.motion.x;
			int local_y = event.motion.y;

			io.MousePos.x = static_cast<float>(local_x);
			io.MousePos.y = static_cast<float>(local_y);

			return true;
		}
		case SDL_MOUSEWHEEL:
		{
			if (event.wheel.x > 0) io.MouseWheelH += 1;
			if (event.wheel.x < 0) io.MouseWheelH -= 1;
			if (event.wheel.y > 0) io.MouseWheel  += 1;
			if (event.wheel.y < 0) io.MouseWheel  -= 1;

			return true;
		}
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			int button = 0;
			bool is_press = (event.type == SDL_MOUSEBUTTONDOWN);

			if (event.button.button == SDL_BUTTON_LEFT)   button = 0;
			if (event.button.button == SDL_BUTTON_RIGHT)  button = 1;
			if (event.button.button == SDL_BUTTON_MIDDLE) button = 2;
			if (event.button.button == SDL_BUTTON_X1)     button = 3; // not used by imgui as a rule
			if (event.button.button == SDL_BUTTON_X2)     button = 4; // not used by imgui as a rule

			io.MouseDown[button] = is_press;

			return true;
		}
		case SDL_TEXTINPUT:
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddInputCharactersUTF8(event.text.text);
			return true;
		}
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			int key = event.key.keysym.scancode;
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));

			io.KeysDown[key] = (event.key.type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl  = ((SDL_GetModState() & KMOD_CTRL)  != 0);
			io.KeyAlt   = ((SDL_GetModState() & KMOD_ALT)   != 0);
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI)   != 0);

			Keyboard.Switchers.KeyAlt   = io.KeyAlt;
			Keyboard.Switchers.KeyCtrl  = io.KeyCtrl;
			Keyboard.Switchers.KeyShift = io.KeyShift;
			Keyboard.Switchers.KeySyper = io.KeySuper;

			return true;
		}
		}

		return false;
	}
};