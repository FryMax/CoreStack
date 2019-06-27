#pragma once

#include "globals.h"
#include "art.bitmap.h"
#include "os.h"

LIB_NAMESPACE_START

namespace os
{
	struct InputEvent { };

	struct MouseEvent : public virtual InputEvent
	{
		MouseEvent(int _x, int _y) : x(_x), y(_y) {}
		int x;
		int y;
	};

	struct MouseClickEvent : public virtual InputEvent, public MouseEvent
	{
 		MouseClickEvent(MKey _buttons, bool IsPress, int _x, int _y) :
			MouseEvent(_x, _y), mk_key(_buttons), is_click(IsPress) {}

		MKey mk_key;
		bool is_click;
	};

	struct MouseWheelEvent : public virtual InputEvent, public MouseEvent
	{
		MouseWheelEvent(int _x_wheel, int _y_wheel, int _x, int _y) :
			MouseEvent(_x, _y), wheel_x(_x_wheel), wheel_y(_y_wheel) {}

		int wheel_x;
		int wheel_y;
	};

	struct KeyEvent : public virtual InputEvent
	{
		KeyEvent(KKey _kk, char32_t _wc, bool _press) :
			kk_key(_kk), wc_key(_wc), is_press(_press) {}

		KKey     kk_key; // key id
		char32_t wc_key; // unicode key value
		bool     is_press;
	};

	//-----------------------------------------------------------------//

	class interfaceInputHandler
	{
	public:
		virtual ~interfaceInputHandler() {}

		virtual void MouseMove    (const MouseEvent&)      { };
		virtual void MouseWheel   (const MouseWheelEvent&) { };
		virtual void MousePress   (const MouseClickEvent&) { };
		virtual void MouseRelease (const MouseClickEvent&) { };
		virtual void KeyPress     (const KeyEvent&)        { };
		virtual void KeyReleas    (const KeyEvent&)        { };

		// Note:
		//  should return true if event are processed and
		//  shouldn't be transmitted to other os handlers
		//
		enum EV_STATUS
		{
			EV_PROCESSED,
			EV_NOT_PROCESSED,
		};

		virtual EV_STATUS ExInputHandler (const os_window_event_msg& ) { return EV_NOT_PROCESSED; };
		//virtual void SetWindow			 (const os_window_handle&    ) { };
		//protected:
		//os_window_handle my_window;
	};

	class WindowInputLisetner
	{
	protected:
		std::vector<interfaceInputHandler*> mListeners;
		bool mInputActive = false;
		INT2 mCursorPos   = {0,0};
		QUAD mWindowRect  = {0,0,0,0};

	public:
		virtual ~WindowInputLisetner() {}

		// add window input listner
		bool AddListener(interfaceInputHandler* listener)
		{
			if (listener)
				mListeners.push_back(listener);

			return listener;
		}

		// any window event
		bool EventMSG(const os_window_event_msg& msg)
		{
			if (!mInputActive)
				return false;

			return api_process_window_event(msg);
		}

		// input window event
		bool InputMSG(const os_window_event_msg& msg)
		{
			if (!mInputActive)
				return false;

			return api_process_input_msg(msg);
		}

		//-------------------------------------------------------------

		void StartInputHandle() { mInputActive = true;  }
		void StopInputHandle () { mInputActive = false;	}

		//-------------------------------------------------------------

		// shoud be called when window rect is changed
		void UpdateWindowRect(const QUAD& rect) { mWindowRect = rect; }

		// cursor pos shoud be updates on eny input event?
		void UpdateGlobalMousePos(INT2 pos)
		{
			const INT2 LocalPos = pos - mWindowRect.GetPos();

			if (mCursorPos != LocalPos)
			{
				mCursorPos = LocalPos;
				PushEvent_MMove(mCursorPos);
			}
		};

		//------ mouse

		void PushEvent_MMove(INT2 pos)
		{
			for (const auto& it : mListeners)
				it->MouseMove(MouseEvent(mCursorPos.x, mCursorPos.y));
		};

		void PushEvent_MPress(const MKey mk, bool is_press)
		{
			for (const auto& it : mListeners)
			{
				if (is_press == true)
					it->MousePress  (MouseClickEvent(mk, true, mCursorPos.x, mCursorPos.y));
				else
					it->MouseRelease(MouseClickEvent(mk, false, mCursorPos.x, mCursorPos.y));
			}
		}

		void PushEvent_MWheel(short x_value, short y_value)
		{
			for (const auto& it : mListeners)
				it->MouseWheel(MouseWheelEvent(x_value, y_value, mCursorPos.x, mCursorPos.y));
		}

		//------ keyboard

		void PushEvent_KKey(const KKey keyId, const char32_t keyUnicode, bool isPress)
		{
			for (const auto& it : mListeners)
				if (isPress)
					it->KeyPress (KeyEvent(keyId, keyUnicode, isPress));
				else
					it->KeyReleas(KeyEvent(keyId, keyUnicode, isPress));
		}

	protected:
		virtual bool api_process_window_event(const os_window_event_msg& msg)
		{
			bool processed = false;

			for (const auto& it : mListeners)
				processed |= it->ExInputHandler(msg) == interfaceInputHandler::EV_PROCESSED;

			return processed;
		}

		virtual bool api_process_input_msg(const os_window_event_msg& msg);
	};

}

LIB_NAMESPACE_END

#if (PLATFORM_OS == OS_WINDOWS)

	inline bool os::WindowInputLisetner::api_process_input_msg(const os_window_event_msg& msg)
	{
		UINT	ev = msg.msg;
		WPARAM	wp = msg.wp;
		LPARAM	lp = msg.lp;

		this->UpdateGlobalMousePos(os_api::get_cursor_pos());

		switch (ev)
		{
		//------------------------------------  keyboard
		case WM_KEYDOWN:    case WM_KEYUP:
		case WM_SYSKEYDOWN: case WM_SYSKEYUP:
		{
			const bool IsPress  = ev == WM_KEYDOWN || ev == WM_SYSKEYDOWN;
			const KKey KeyIndex = static_cast<KKey>(wp);

			BYTE KbState[256];
			if (::GetKeyboardState(KbState) == TRUE)
			{
				wchar_t UnicodeChar;
				::ToUnicode(UINT(wp), HIWORD(lp) & 0xFF, KbState, &UnicodeChar, 1, 0);

				PushEvent_KKey(KeyIndex, UnicodeChar, IsPress);
			}
			break;
		}
		//------------------------------------  mice
		case WM_LBUTTONDOWN: PushEvent_MPress(MKey::MOUSE_LEFT,   true ); break;
		case WM_LBUTTONUP:   PushEvent_MPress(MKey::MOUSE_LEFT,   false); break;
		case WM_RBUTTONDOWN: PushEvent_MPress(MKey::MOUSE_RIGHT,  true ); break;
		case WM_RBUTTONUP:   PushEvent_MPress(MKey::MOUSE_RIGHT,  false); break;
		case WM_MBUTTONDOWN: PushEvent_MPress(MKey::MOUSE_MIDDLE, true ); break;
		case WM_MBUTTONUP:   PushEvent_MPress(MKey::MOUSE_MIDDLE, false); break;
		case WM_MOUSEWHEEL:  PushEvent_MWheel(GET_WHEEL_DELTA_WPARAM(wp) / WHEEL_DELTA, 0); break;
	#if (_WIN32_WINNT >= 0x0600 || defined(WM_MOUSEHWHEEL))
		case WM_MOUSEHWHEEL: PushEvent_MWheel(0, GET_WHEEL_DELTA_WPARAM(wp) / WHEEL_DELTA); break;
	#endif
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		{
			static BOOL FState = false;
			static BOOL BState = false;
			const  bool IsPress = (WM_XBUTTONDOWN == ev);

			if (FState != ::GetAsyncKeyState(VK_XBUTTON2))
			{
				FState = !FState;
				PushEvent_MPress(MKey::MOUSE_FORWARD, IsPress);
			}

			if (BState != ::GetAsyncKeyState(VK_XBUTTON1))
			{
				BState = !BState;
				PushEvent_MPress(MKey::MOUSE_BUCK, IsPress);
			}

			break;
		}
		//------------------------------------
		}

		const bool is_pocessed = true;
		return is_pocessed;
	}

#else

	inline bool os::WindowInputLisetner::api_process_input_msg(const os_window_event_msg& msg)
	{
		LOG_WARN_NO_IMPL;
		return false;
	}

#endif