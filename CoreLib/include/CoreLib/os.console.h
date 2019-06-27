#pragma once

#include "globals.h"
#include "os.h"
#include "os.api.h"

LIB_NAMESPACE_START

namespace os
{
 	struct ConsoleState
	{
	public:
		friend class Console;

	public:
		std::string Title = "Console";  // [Console]

	protected:
		os_window_handle Hwnd   = {};   // [nullptr]
		os_handle        Handle = {};   // [nullptr]

		bool IsInited      = false;
		bool IsShowed      = false;
		bool IORedirected  = false;
		bool IsUTF8Enabled = false;
	};

	class Console
	{
	protected:
		ConsoleState mCS;

	public:
		bool Init(const ConsoleState& cs)
		{
			mCS = cs;
			return mCS.IsInited = api_init_console();
		}

		bool Show()
		{
			if (!mCS.IsInited)
				return false;

			return mCS.IsShowed = api_show_console();
		}

		bool RedirectIO()
		{
 			if (!mCS.IsShowed)
				return false;

			return mCS.IORedirected = api_redirect_io();
		}

		bool SetUTF8Encoding()
		{
			if (!mCS.IORedirected)
				return false;

			return mCS.IsUTF8Enabled = api_set_enc_utf8();
		}

 	private:
		virtual bool api_init_console ();
		virtual bool api_show_console ();
		virtual bool api_redirect_io  ();
		virtual bool api_set_enc_utf8 ();
	};
}

#if (PLATFORM_OS == OS_WINDOWS)
	/////////////////////////////////////////////////////////////
	inline bool	os::Console::api_init_console()
	{
		assert(nullptr == mCS.Hwnd.hwnd);
		assert(nullptr == mCS.Handle.handle);

		return true;
	}
	/////////////////////////////////////////////////////////////
	inline bool	os::Console::api_show_console()
	{
		if (::AllocConsole() == TRUE)
		{
			mCS.Hwnd.hwnd	  = ::GetConsoleWindow();
			mCS.Handle.handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		}
		else
		{

		}

		return true;
	}
	/////////////////////////////////////////////////////////////
	inline bool	os::Console::api_redirect_io()
	{
		FILE *fptrI;
		FILE *fptrO;
		FILE *fptrE;

		freopen_s(&fptrI, "CONOUT$", "w", stdout);
		freopen_s(&fptrO, "CONOUT$", "r", stdin);
		freopen_s(&fptrE, "CONOUT$", "w", stderr);

		return true;
	}
	/////////////////////////////////////////////////////////////
	inline bool os::Console::api_set_enc_utf8()
	{
		constexpr UINT WIN32_UTF8_CODEPAN = 65001;

		return ::SetConsoleCP(WIN32_UTF8_CODEPAN) == TRUE
			&& ::SetConsoleOutputCP(WIN32_UTF8_CODEPAN) == TRUE;
	}
	/////////////////////////////////////////////////////////////

#else
	inline bool os::Console::api_init_console () { LOG_WARN_NO_IMPL; return true; }
	inline bool os::Console::api_show_console () { LOG_WARN_NO_IMPL; return true; }
	inline bool os::Console::api_redirect_io  () { LOG_WARN_NO_IMPL; return true; }
	inline bool os::Console::api_set_enc_utf8 () { LOG_WARN_NO_IMPL; return true; }
#endif

LIB_NAMESPACE_END