#pragma once

#include "globals.h"
#include "os.api.h"

#include <windows.h>
#include <comdef.h>
#include <dbghelp.h>

//#include <Minidumpapiset.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")

namespace
{
	inline HRESULT HRESULT_FROM_WIN32_ERROR(unsigned long x)
	{
		return HRESULT_FROM_WIN32(x);
	}

	inline std::wstring WSTRING_FROM_HRESULT(HRESULT hr)
	{
		return (_com_error(hr)).ErrorMessage();
	}

	inline std::string STRING_FROM_ERROR(DWORD Error = ::GetLastError())
	{
		if (Error == 0)
			return std::string(); //No error message has been recorded

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, Error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	};

	inline RESULT& operator<<(RESULT& OK, HRESULT hr)
	{
		OK.is_ok = bool(SUCCEEDED(hr));

		if (OK.IsFailed())
		{
			std::wstring error_hr    = WSTRING_FROM_HRESULT(hr);
			std::wstring error_win32 = WSTRING_FROM_HRESULT(HRESULT_FROM_WIN32_ERROR(GetLastError()));

			std::string	es     = str::convert::wstring_to_string(error_hr);
			std::string es32   = str::convert::wstring_to_string(error_win32);
			std::string utf8   = str::convert::cp1251_to_utf8(es);
			std::string utf832 = str::convert::cp1251_to_utf8(es32);

			LIB_NAME::log::error << "HR ERR: " + utf8;
			LIB_NAME::log::error << "HR ERR: " + utf832;
		}

		return OK;
	}

	inline RESULT& operator<<(RESULT OK, DWORD win32_error)
	{
		HRESULT hr = HRESULT_FROM_WIN32_ERROR(win32_error);

		return OK << hr;
	}
}



LIB_NAMESPACE_START

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace os_api
{
 	//==================================//
	namespace alt
	{
		inline bool ShowCursor(bool _show, bool _true_cast = true)
		{
			if (_true_cast == true)
			{
				size_t n = 0;

				if (_show)	while (::ShowCursor(TRUE)  <= 0) n++;
				else		while (::ShowCursor(FALSE) >= 0) n++;

				const size_t EAC = 3;
				if (n > EAC)
					DEBUG_BREAK();

				return true;
			}
			else
				return ::ShowCursor(_show);
		}

		inline INT2 GetCursorPos()
		{
			POINT pos;
 			if (::GetCursorPos(&pos))
				return INT2(pos.x, pos.y);
			else
				return INT2{ 0,0 };
		}
	}

	//=====================================================// System

	inline std::string get_curent_path()
	{
		char result[MAX_PATH];
		return std::string(result, GetModuleFileNameA(NULL, result, MAX_PATH));
	}

	inline void sleep_mms(uint64_t time_mms)
	{
		::Sleep((DWORD)time_mms);
	}

	inline bool set_thread_name(os_thread_id th, const char* name)
	{

		const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // Must be 0x1000.
			LPCSTR szName; // Pointer to name (in user addr space).
			DWORD dwThreadID; // Thread ID (-1=caller thread).
			DWORD dwFlags; // Reserved for future use, must be zero.
		} THREADNAME_INFO;
#pragma pack(pop)
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = th.id;
        info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
		__try {
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}
#pragma warning(pop)

		//std::string s(name);
		//std::wstring ws(s.begin(), s.end());
		//
		//RESULT ok;
		//ok << ::SetThreadDescription(GetCurrentThread(), ws.data());
		return true;
	}

	inline result< int32_t> get_system_metric(os_sys_metric sm)
	{
		switch (sm)
		{
		case os_sys_metric::VISIBLE_MONITOR_COUNT: return ::GetSystemMetrics(SM_CMONITORS);
		case os_sys_metric::MAIN_MONITOR_W:        return ::GetSystemMetrics(SM_CXSCREEN);
		case os_sys_metric::MAIN_MONITOR_H:        return ::GetSystemMetrics(SM_CYSCREEN);

		default: break;
		}

		DEBUG_BREAK();
		return error_result( 0, "Failed to get specified system metric");
	}

	inline result<uint64_t> get_ram_metric(os_ram_metric sm)
	{
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(::GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

		switch (sm)
		{
		case os_ram_metric::MEM_MemoryLoadProcent:         return memInfo.dwMemoryLoad;
		case os_ram_metric::MEM_PageFileSize:              return memInfo.ullTotalPageFile;
		case os_ram_metric::MEM_PageFileLoad:              return memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
		case os_ram_metric::MEM_PageFileAvail:             return memInfo.ullAvailPageFile;
		case os_ram_metric::MEM_VirtualMemoryTotal:        return memInfo.ullTotalVirtual;
		case os_ram_metric::MEM_VirtualMemoryLoad:         return memInfo.ullTotalVirtual - memInfo.ullAvailVirtual;
		case os_ram_metric::MEM_VirtualMemoryAvail:        return memInfo.ullAvailVirtual;
		case os_ram_metric::MEM_RealMemoryTotal:           return memInfo.ullTotalPhys;
		case os_ram_metric::MEM_RealMemoryLoad:            return memInfo.ullTotalPhys - memInfo.ullAvailPhys;
		case os_ram_metric::MEM_RealMemoryAvail:           return memInfo.ullAvailPhys;
		case os_ram_metric::MEM_CurentProcVirtualMemUsage: return pmc.PrivateUsage;
		case os_ram_metric::MEM_CurentProcRealMemUsage:    return pmc.WorkingSetSize;
		default:
			break;
		}

		DEBUG_BREAK();
		return error_result(0ULL, "Failed to get specified ram metric");
	}

	inline result< int32_t> get_cpu_metric(os_cpu_metric sm)
	{

		switch (sm)
		{
		case os_cpu_metric::CPU_CpuLoadProcent:	-1;
		case os_cpu_metric::CPU_CpuNodeCount:	-1;
		case os_cpu_metric::CPU_CpuCoreCount:	-1;
		case os_cpu_metric::CPU_CpuThreadCount:	return -1; //std::thread::hardware_concurrency();

		default:
			break;
		}

		DEBUG_BREAK();
		return error_result(0, "Failed to get selected cpu metric");
	}

	//-----------------------------------------------------------------------------
	//  Cursor
	//-----------------------------------------------------------------------------

	inline bool show_cursor   ()         { return alt::ShowCursor(true);           }
	inline bool hide_cursor   ()         { return alt::ShowCursor(false);          }
	inline INT2 get_cursor_pos()         { return alt::GetCursorPos();             }
	inline bool set_cursor_pos(INT2 pos) { return    ::SetCursorPos(pos.x, pos.y); }

 	//-----------------------------------------------------------------------------
	//  Window API
	//-----------------------------------------------------------------------------

	inline bool show_window(os_window_handle window)
	{
		return ::ShowWindow(window.hwnd, SW_SHOW);
	}

	inline bool hide_window(os_window_handle window)
	{
		return ::ShowWindow(window.hwnd, SW_HIDE);
	}

	inline bool move_window(os_window_handle window, INT2 pos, INT2 size)
	{
		return ::MoveWindow(window.hwnd, pos.x, pos.y, size.x, size.y, true);
	}

	inline bool destroy_window(os_window_handle window)
	{
		return ::DestroyWindow(window.hwnd);
	}

	inline bool get_window_rect(os_window_handle window, window_canvas_rect* rect)
	{
		RECT  rec;
		POINT g_p0;
		POINT g_p1;

		::GetClientRect(window.hwnd, &rec);
		g_p0.x = rec.left;
		g_p0.y = rec.top;
		g_p1.x = rec.right;
		g_p1.y = rec.bottom;
		::ClientToScreen(window.hwnd, &g_p0);
		::ClientToScreen(window.hwnd, &g_p1);

		rect->loc_x       = 0;
		rect->loc_y       = 0;
		rect->loc_w       = rec.right;
		rect->loc_h       = rec.bottom;
		rect->glob_left   = g_p0.x;
		rect->glob_top    = g_p0.y;
		rect->glob_right  = g_p1.x;
		rect->glob_bot    = g_p1.y;

		return true;
	}

	inline bool set_window_z(os_window_handle window, window_z_depth depth)
	{
		switch (depth)
		{
		case window_z_depth::Z_TOP:     return ::SetWindowPos(window.hwnd, HWND_TOP,       0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		case window_z_depth::Z_BOTTOM:  return ::SetWindowPos(window.hwnd, HWND_BOTTOM,    0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		case window_z_depth::Z_TOPMOST: return ::SetWindowPos(window.hwnd, HWND_TOPMOST,   0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		case window_z_depth::Z_NORMAL:  return ::SetWindowPos(window.hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		return true;
	}

	inline bool set_window_pos(os_window_handle window, INT2 pos)
	{
		return ::SetWindowPos(window.hwnd, 0, pos.x, pos.y, 0, 0, SWP_NOSIZE);
	}

	inline bool set_window_title(os_window_handle window, std::string title)
	{
		return ::SetWindowTextA(window.hwnd, title.c_str());
	}

	// mb dell

	static bool SetOutputConsoleTitle(std::string title)
	{
		return ::SetConsoleTitleA(title.c_str());
	}

	static bool SetOutputConsoleColors(int textc, int buckc)
	{
		return ::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)((buckc << 4) | textc));
	}

	static bool SetOutputConsoleFont(std::wstring f, int w, int h)
	{
		//CONSOLE_FONT_INFOEX  inf;
		//inf.cbSize = sizeof(CONSOLE_FONT_INFOEX);
		//inf.nFont = 2;
		//inf.dwFontSize = { (short)w,(short)h };
		//inf.FontFamily = 48;
		//inf.FontWeight = 700;
		//std::wcsncpy(inf.FaceName, f.c_str(), 32);
		//return ::SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &inf);
		LOG_WARN_NO_IMPL;
		return false;
	}

	namespace Win32DBG
	{
		namespace
		{
			inline void win_make_minidump(EXCEPTION_POINTERS* e)
			{
				const auto dbghelp_lib = LoadLibraryA("dbghelp");

				if (dbghelp_lib == nullptr)
					return;

				const auto fpMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(dbghelp_lib, "MiniDumpWriteDump");

				if (fpMiniDumpWriteDump == nullptr)
					return;

                std::string exePath(MAX_PATH, 0);
                exePath.resize(GetModuleFileNameA(GetModuleHandleA(0), &exePath[0], MAX_PATH) - strlen(".exe"));

                SYSTEMTIME t;
                GetSystemTime(&t);
				const std::string dumpPath = exePath + str::format::insert(" {}.{}.{} {}:{}.{}.{}.dmp",
                    t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

				const HANDLE hFile = CreateFileA(dumpPath.data(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile == INVALID_HANDLE_VALUE)
					return;

				MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
				exceptionInfo.ThreadId = GetCurrentThreadId();
				exceptionInfo.ExceptionPointers = e;
				exceptionInfo.ClientPointers = FALSE;

				const BOOL dumped = fpMiniDumpWriteDump(
					GetCurrentProcess(),
					GetCurrentProcessId(),
					hFile,
					MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
					e ? &exceptionInfo : nullptr,
					nullptr,
					nullptr);

				CloseHandle(hFile);

				return;
			}
			LONG CALLBACK win_unhandled_handler(EXCEPTION_POINTERS* e)
			{
				win_make_minidump(e);
				return EXCEPTION_CONTINUE_SEARCH;
			}
		}

		inline void OnUeCreateMiniDump()
		{
			::SetUnhandledExceptionFilter(win_unhandled_handler);
		}
		inline int MakeFakeExpeption()
		{
			return *(int*)0;
		}

	}

	inline void AttachCrashHandler()
	{
		Win32DBG::OnUeCreateMiniDump();
	}

};

LIB_NAMESPACE_END