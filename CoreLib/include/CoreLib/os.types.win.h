#pragma once

#include "globals.h"
#include "os.types.h"
#include <Windows.h>

LIB_NAMESPACE_START

namespace os
{
	struct os_thread_id     { ULONG   id;     };
	struct os_handle        { HANDLE  handle; };
	struct os_window_handle { HWND    hwnd;   };
	struct os_module        { HMODULE module; };

	struct os_window_event_msg
	{
		os_window_event_msg(HWND _h, UINT _u, WPARAM _wp, LPARAM _lp) :
			hwnd(_h), msg(_u), wp(_wp), lp(_lp), padding("")
		{}

		os_window_event_msg()
		{}

		HWND	hwnd = 0;
		UINT	msg  = 0;
		WPARAM	wp   = 0;
		LPARAM	lp   = 0;

		bool todo_processed_by_pipeline = false;

	private:
		volatile char padding[64] = "";
	};

}

LIB_NAMESPACE_END