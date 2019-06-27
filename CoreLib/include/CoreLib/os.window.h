#pragma once

#include "globals.h"
#include "math.h"

#include "os.h"
#include "os.api.h"
#include "os.input.h"

LIB_NAMESPACE_START

namespace
{
	using namespace os;
	using namespace os_api;
}

namespace os
{
	//===================================================//  stile

	enum class WindowBorderStile : size_t
	{
		DefaultBorders,
		Borderless,
		BorderlessFullScr,
		_enum_size,
	};
	enum class WindowAlphaStile : size_t
	{
		NoAlpha,
		AlphaChanel,
		ColorKey,
		_enum_size,
	};
	enum class WindowTransperentStile : size_t
	{
		Opaque,
		Transparent,
		_enum_size,
	};
	enum class WindowSpawnPos : size_t
	{
		XY,
		CenterOfMainMonitor,
		RandomInMainMonitor,
		_enum_size,
	};

	//===================================================//

	struct WindowState
	{
		WindowState& operator=(const WindowState& state) = default;

	//------ editable

		WindowBorderStile      stile_borders     = WindowBorderStile::DefaultBorders;
		WindowAlphaStile       stile_alpha       = WindowAlphaStile::NoAlpha;
		WindowTransperentStile stile_transperent = WindowTransperentStile::Opaque;
		WindowSpawnPos         stile_spawn       = WindowSpawnPos::CenterOfMainMonitor;

		std::string Title = "Window"; //  Window title

		int      X        = 64;       //  [64]     client rect
		int      Y        = 64;       //  [64]     client rect
		unsigned Wight    = 800;      //  [800]    client rect
		unsigned Height   = 600;      //  [600]    client rect
		bool     Resizble = true;     //  [true]   drag? resize allowed
		bool     Visable  = false;    //  [false]  is visable
		bool     TopMost  = false;

	//------ todo

		//unsigned transperent_value = 255;

	//------ edit not expected

		os_window_handle osWindow;

	};

	class Window : public NonAssignable
	{
	protected:
		WindowState mState;
		WindowInputLisetner	mInput;

		// mb move to window state //
		bool isExitSms    = false; //
		bool isActive     = false; //
		bool isMinimized  = false; //
		bool isMaximized  = false; //
		bool isResized    = false; //

	private:
		static Window* _WindowSetter(Window* window = nullptr)
		{
			static Window* sg_window = nullptr;

			if (window)
				sg_window = window;

			return sg_window;
		}

	public:
		virtual ~Window() {}

		static Window* Get()
		{
			return _WindowSetter(nullptr);
		}

		bool Init(const WindowState& WS)
		{
			mState = WS;
			_WindowSetter(this);

			return api_create_window();
		}

		void Close(bool imidiate = true)
		{
			isExitSms = true;

			if (imidiate)
				api_destroy_window();
		}

		void HandleMessages()
		{
			api_handle_messages();
		}

		bool AttachInputHandler(interfaceInputHandler* input)
		{
			if (!input)
				return false;

			mInput.AddListener(input);
			mInput.StartInputHandle();
			updateInputRect();

			return true;
		}

		//------------------------------------------------------
		//  Stile
		//------------------------------------------------------

		bool Show() { mState.Visable = true;  return api_show_hide(true);  }
		bool Hide() { mState.Visable = false; return api_show_hide(false); }

		bool SetTopMost(bool always_top)
		{
			mState.TopMost = always_top;
			return api_topmost(always_top);
		}

		bool SetWindowFrame(WindowBorderStile wbs)
		{
			mState.stile_borders = wbs;
			return api_window_stile(wbs);
		}

	public:

		inline const WindowState* GetWS()       const { return &mState; }
		inline os_window_handle	  GetWindowId() const { return mState.osWindow; }

		inline std::string GetTitle    () const { return mState.Title;  }
		inline int         GetX        () const { return mState.X;      }
		inline int         GetY        () const { return mState.Y;      }
		inline size_t      GetWidth    () const { return mState.Wight;  }
		inline size_t      GetHeight   () const { return mState.Height; }
		inline bool        IsMaximized () const { return isMaximized;   }
		inline bool        IsMinimized () const { return isMinimized;   }
		inline bool        IsExit      () const { return isExitSms;     }
		inline bool        IsActive    () const { return isActive;      } // Returns true if window focused?

		// Returns true once after each window resize
		inline bool WasResized () { return std::exchange(isResized, false); }

	protected:

		void updateInputRect()
		{
			const INT2 pos  = INT2(mState.X, mState.Y);
			const INT2 size = INT2(mState.Wight, mState.Height);

			mInput.UpdateWindowRect(QUAD(pos, size));
  		}

	//------------------------------------------------------
	//  API
	//------------------------------------------------------

		virtual bool api_create_window   ();
		virtual void api_destroy_window  ();
		virtual void api_handle_messages ();

		virtual bool api_show_hide    (bool isVisable);
		virtual bool api_topmost      (bool isTopmost);
		virtual bool api_window_stile (WindowBorderStile wbs);

	}; // class Window

} // namespace os

LIB_NAMESPACE_END


#if PLATFORM_OS == OS_WINDOWS

	struct WindowAccessor : os::Window
	{
		static LRESULT CALLBACK fnMsgHandler(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
		{
			//todo mb multiple windows
			WindowAccessor* win = reinterpret_cast<WindowAccessor*>(::Window::Get());
			auto os_sms = os_window_event_msg(hwnd, nMsg, wParam, lParam);

			return WindowMsgHandler(*win, os_sms);
		}

		static LRESULT WindowMsgHandler(WindowAccessor& Win, const os_window_event_msg& sms)
		{
			const HWND hwnd = sms.hwnd;
			const UINT nMsg = sms.msg;
			const WPARAM wParam = sms.wp;
			const LPARAM lParam = sms.lp;

			// send to input reader first
			bool processed = Win.mInput.EventMSG(sms);

			switch (nMsg)
			{
			case WM_CREATE:
				// mb add here surface params
				break;

			case WM_CLOSE:
				Win.isExitSms = true;
				break;

			case WM_ACTIVATE:
				Win.isActive = (LOWORD(wParam) != WA_INACTIVE) ? true : false;
				break;

			case WM_MOVE:
				Win.mState.X = (short)LOWORD(lParam);
				Win.mState.Y = (short)HIWORD(lParam);
				Win.updateInputRect();
				break;

  			case WM_SIZE:
				Win.isResized     = true;
				Win.mState.Wight  = (short)LOWORD(lParam);
				Win.mState.Height = (short)HIWORD(lParam);

				if (wParam == SIZE_MINIMIZED)
				{
					Win.isActive    = false;
					Win.isMaximized = false;
					Win.isMinimized = true;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					Win.isActive    = true;
					Win.isMaximized = true;
					Win.isMinimized = false;
				}
				else if (wParam == SIZE_RESTORED)
				{
					if(Win.isMinimized)
					{
						Win.isActive    = true;
						Win.isMinimized = false;
					}
					else if (Win.isMaximized)
					{
						Win.isActive    = true;
						Win.isMaximized = false;
					}
				}

				Win.updateInputRect();
				break;

			//---------------------------------------------------- Actions

			case WM_SETCURSOR:
				//log::debug << __FUNCTION__ ": WM_SETCURSOR (no handler)";
				break;

			case WM_DROPFILES:
				log::debug << __FUNCTION__ ": WM_DROPFILES (no handler)";
				break;

			//----------------------------------------------------// Events for input handler

			//////  MOUSY  //////

			case WM_MOUSEMOVE:
			case WM_MOUSEWHEEL:
	#if (_WIN32_WINNT >= 0x0600)
			case WM_MOUSEHWHEEL: // hor weel
	#endif
			case WM_LBUTTONDOWN: case WM_LBUTTONUP:
			case WM_MBUTTONDOWN: case WM_MBUTTONUP:
			case WM_RBUTTONDOWN: case WM_RBUTTONUP:
			case WM_XBUTTONDOWN: case WM_XBUTTONUP:

				//////  KEYBOARD  //////

			case WM_KEYDOWN:	 case WM_KEYUP:
			case WM_SYSKEYDOWN:	 case WM_SYSKEYUP:
			case WM_CHAR:

				Win.mInput.InputMSG(sms);
				break;

			default:
				0;
			}

			// ANOTHER WINDOW MESSAGES

			//todo check this (disable alt behavior)
			if (nMsg == WM_SYSCOMMAND)
			{
				if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
					processed = true;
			}

			return processed ? 0 : ::DefWindowProc(hwnd, nMsg, wParam, lParam);
		}
	};

    inline void os::Window::api_handle_messages()
    {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
			TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	inline bool os::Window::api_create_window()
	{

	#ifdef IDI_APP_ICON
		#define WIN32_WINDOW_LOGO LoadIcon(ws.Hmodule, MAKEINTRESOURCE(IDI_APP_ICON))
	#else
		#define WIN32_WINDOW_LOGO LoadIcon(NULL, IDI_WINLOGO)
	#endif

		const std::wstring window_class_name = L"ws-window-class";

		WNDCLASSEXW wndClass;

		wndClass.cbSize         = sizeof(WNDCLASSEXW);
		wndClass.style          = CS_HREDRAW | CS_VREDRAW;	//TODO
		wndClass.lpfnWndProc    = WindowAccessor::fnMsgHandler;
		wndClass.cbClsExtra     = 0;
		wndClass.cbWndExtra     = 0;
		wndClass.hInstance      = 0; //mWS.osHmodule.module;
		wndClass.hIcon          = 0; //LoadIcon(mWS.osHmodule.module, MAKEINTRESOURCE(0));//APP_WINDOW_LOGO; IDI_ICON1
		wndClass.hIconSm        = 0; //LoadIcon(mWS.osHmodule.module, MAKEINTRESOURCE(0));//APP_WINDOW_LOGO;//LoadIcon(ws._Hmodule.module, MAKEINTRESOURCE(1));//LoadIcon(ws._Hmodule.module, MAKEINTRESOURCE(101));//APP_WINDOW_LOGO;
		wndClass.hCursor        = ::LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground  = (HBRUSH)::GetStockObject(WHITE_BRUSH);
		wndClass.lpszMenuName   = NULL;
		wndClass.lpszClassName  = window_class_name.c_str();
		wndClass.cbSize         = sizeof(WNDCLASSEX);

		//from http://www.firststeps.ru/mfc/detail/r.php?24
		//
		WS_BORDER;          //  Создание окна с рамкой.
		WS_CAPTION;         //  Создание окна с заголовком (невозможно использовать одновременно со стилем WS_DLGFRAME).
		WS_CHILD;           //
		WS_CHILDWINDOW;     //  Создание дочернего окна (невозможно использовать одновременно со стилем WS_POPUP).
		WS_CLIPCHILDREN;    //  Исключает область, занятую дочерним окном, при выводе в родительское окно.
		WS_CLIPSIBLINGS;    //  Используется совместно со стилем WS_CHILD для отрисовки в дочернем окне областей клипа, перекрываемых другими окнами.
		WS_DISABLED;        //  Создает окно, которое недоступно.
		WS_DLGFRAME;        //  Создает окно с двойной рамкой, без заголовка.
		WS_GROUP;           //  Позволяет объединять элементы управления в группы.
		WS_HSCROLL;         //  Создает окно с горизонтальной полосой прокрутки.
		WS_MAXIMIZE;        //  Создает окно максимального размера.
		WS_MAXIMIZEBOX;     //  Создает окно с кнопкой развертывания окна.
		WS_MINIMIZE;        //
		WS_ICONIC;          //  Создает первоначально свернутое окно (используется только со стилем WS_OWERLAPPED).
		WS_MINIMIZEBOX;     //  Создает окно с кнопкой свертывания.
		WS_OVERLAPPED;      //  Создает перекрывающееся окно (которое, как правило, имеет заголовок и WS_TILED	рамку).
		WS_OVERLAPPEDWINDOW;//  Создает перекрывающееся окно, имеющее стили WS_OVERLAPPED, WS_CAPTION, WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, WS_MAXIMIZEBOX.
		WS_POPUP;           //  Создает popup-окно (невозможно использовать совместно со стилем WS_CHILD.
		WS_POPUPWINDOW;     //  Создает popup-окно, имеющее стили WS_BORDER, WS_POPUP, WS_SYSMENU.
		WS_SYSMENU;         //  Создает окно с кнопкой системного меню (можно использовать только с окнами имеющими строку заголовка).
		WS_TABSTOP;         //  Определяет элементы управления, переход к которым может быть выполнен по клавише TAB.
		WS_THICKFRAME;      //  Создает окно с рамкой, используемой для изменения
		WS_SIZEBOX;         //  размера окна.
		WS_VISIBLE;         //  Создает первоначально неотображаемое окно.
		WS_VSCROLL;         //  Создает окно с вертикальной полосой прокрутки.

		int SpawnXpos = 0;
		int SpawnYpos = 0;

		switch (mState.stile_spawn)
		{
		case WindowSpawnPos::XY:

			SpawnXpos = mState.X;
			SpawnYpos = mState.Y;
			break;

		case WindowSpawnPos::CenterOfMainMonitor:

			SpawnXpos = (os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_W).GetValue() / 2) - (mState.Wight  / 2);
			SpawnYpos = (os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_H).GetValue() / 2) - (mState.Height / 2);
			break;

		case WindowSpawnPos::RandomInMainMonitor:

			SpawnXpos = (int)math::RandomInt64(0, os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_W).GetValue() - mState.Wight);
			SpawnYpos = (int)math::RandomInt64(0, os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_H).GetValue() - mState.Height);
			break;
		}

 		RECT  rect	= { 0, 0, (LONG)mState.Wight, (LONG)mState.Height };
 		DWORD Stile	= WS_OVERLAPPEDWINDOW;

		if ("Stile")
		{
			if (!mState.Resizble) Stile |= WS_THICKFRAME;
			if (mState.Visable)   Stile |= WS_VISIBLE;
 		}

        switch (mState.stile_borders)
        {
        case WindowBorderStile::DefaultBorders:
        case WindowBorderStile::Borderless:
			0;//todo
        }
        switch (mState.stile_alpha)
        {
        case WindowAlphaStile::NoAlpha:
        case WindowAlphaStile::AlphaChanel:
        case WindowAlphaStile::ColorKey:
			0;//todo
        }
        switch (mState.stile_transperent)
        {
        case WindowTransperentStile::Opaque:
        case WindowTransperentStile::Transparent:
			0;//todo
		}

 		::AdjustWindowRect(&rect, Stile, FALSE);

		int Wsize = (int)rect.right - rect.left;
		int Hsize = (int)rect.bottom - rect.top;
		std::wstring caption(mState.Title.begin(), mState.Title.end()); //= str::convert::string_to_wstring(mState.Title);

		if (!::RegisterClassEx(&wndClass))
			return false;

		HWND hwnd = ::CreateWindowEx(
			NULL,
			window_class_name.c_str(),
			caption.c_str(),
			Stile,
			SpawnXpos,
			SpawnYpos,
			Wsize,
			Hsize,
			NULL,
			NULL,
			NULL,//mWS.osHmodule.module,
			NULL);

		RESULT OK;
		OK << ::GetLastError();

		if (hwnd)
			mState.osWindow.hwnd = hwnd;
		else
			return false;

		return true;
	}

	inline void os::Window::api_destroy_window()
	{
		os_api::destroy_window(mState.osWindow);
	}

	inline bool os::Window::api_show_hide(bool isVisable)
	{
		if (isVisable)
			return os_api::show_window(mState.osWindow);
		else
			return os_api::hide_window(mState.osWindow);
	}

	inline bool os::Window::api_topmost(bool isTopmost)
	{
		if (isTopmost)
			return os_api::set_window_z(mState.osWindow, os::window_z_depth::Z_TOPMOST);
		else
			return os_api::set_window_z(mState.osWindow, os::window_z_depth::Z_NORMAL);
	}

	inline bool os::Window::api_window_stile(WindowBorderStile newStile)
	{
		static INT2 NormalPos;
		static INT2 NormalSize;
		static LONG NormalStile;
		static LONG OverlapStyle;
		static LONG BorderlessStile;

		MakeOnce
		{
			NormalPos       = { this->GetX(),	  this->GetY() };
			NormalSize      = { this->GetWidth(), this->GetHeight() };
			NormalStile     = ::GetWindowLong(GetWindowId().hwnd, GWL_STYLE);
			OverlapStyle    = WS_OVERLAPPEDWINDOW;
			BorderlessStile = WS_POPUP;
		}

		static WindowBorderStile last_stile = newStile;

		if (newStile == WindowBorderStile::DefaultBorders)
		{
			this->SetTopMost(false);

			::SetWindowLong(this->GetWindowId().hwnd, GWL_STYLE, NormalStile | OverlapStyle);
			::SetWindowPos(this->GetWindowId().hwnd, NULL,
				NormalPos.x,
				NormalPos.y,
				NormalSize.x,
				NormalSize.y,
				SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
				//| (last_stile == WindowBorderStile::Borderless ? 0 : SWP_NOMOVE | SWP_NOSIZE) );
		}

		if (newStile == WindowBorderStile::Borderless)
		{
			this->SetTopMost(false);

			::SetWindowLong(this->GetWindowId().hwnd, GWL_STYLE,
				(NormalStile | OverlapStyle)
				//& ~(BorderlessStile));
				& ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU));

			::SetWindowPos(this->GetWindowId().hwnd, NULL,
				NormalPos.x,
				NormalPos.y,
				NormalSize.x,
				NormalSize.y,
				SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
				//| (last_stile == WindowBorderStile::Borderless ? 0 : SWP_NOMOVE | SWP_NOSIZE)  );
		}

		if (newStile == WindowBorderStile::BorderlessFullScr)
		{
			this->SetTopMost(true);

			NormalPos  = { this->GetX(),     this->GetY()      };
			NormalSize = { this->GetWidth(), this->GetHeight() };

			unsigned w = os_api::get_system_metric(os::os_sys_metric::MAIN_MONITOR_W).GetValue();
			unsigned h = os_api::get_system_metric(os::os_sys_metric::MAIN_MONITOR_H).GetValue();

			const bool DISABLE_GPU_DRIVER_FULLSCR_TRIGGER = true;
			if (DISABLE_GPU_DRIVER_FULLSCR_TRIGGER)
				h -= 1;

			::SetWindowLong(this->GetWindowId().hwnd, GWL_STYLE,
				(NormalStile | OverlapStyle)
				//& ~(BorderlessStile));
				& ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU));

			::SetWindowPos (this->GetWindowId().hwnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
		}

		last_stile = newStile;

		return true;
	}
#else

	inline bool os::Window::api_create_window   ()						{ LOG_WARN_NO_IMPL; return false; }
	inline void os::Window::api_destroy_window  ()						{ LOG_WARN_NO_IMPL; }
	inline void os::Window::api_handle_messages ()						{ LOG_WARN_NO_IMPL; }
	inline bool os::Window::api_show_hide       (bool isVisable)		{ LOG_WARN_NO_IMPL; return false; }
	inline bool os::Window::api_topmost         (bool isTopmost)		{ LOG_WARN_NO_IMPL; return false; }
	inline bool os::Window::api_window_stile    (WindowBorderStile wbs) { LOG_WARN_NO_IMPL; return false; }

#endif