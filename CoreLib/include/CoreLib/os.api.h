#pragma once

#include "globals.h"

#include "math.h"

#include "str.converter.h"

#if (PLATFORM_OS == OS_WINDOWS)
	#include "os.types.win.h"
#elif (PLATFORM_OS == OS_UNIX)
	#include "os.types.unix.h"
#endif

namespace
{
	using namespace os;
}

#define OS_API inline

LIB_NAMESPACE_START

namespace os_api
{
	OS_API std::string get_curent_path();

	OS_API void sleep_mms(uint64_t);

	OS_API bool set_thread_name(os_thread_id id, const char* name);

	OS_API result< int32_t> get_system_metric (os_sys_metric sm);
	OS_API result<uint64_t> get_ram_metric    (os_ram_metric rm);
	OS_API result< int32_t> get_cpu_metric    (os_cpu_metric cm);

	//=====================================================// cursor

	OS_API bool show_cursor();
	OS_API bool hide_cursor();
	OS_API INT2 get_cursor_pos();
	OS_API bool set_cursor_pos(INT2  pos);

	//=====================================================// window

	OS_API bool show_window       (os_window_handle window);
	OS_API bool hide_window       (os_window_handle window);

	OS_API bool set_window_z      (os_window_handle window, window_z_depth depth);

	OS_API bool set_window_pos    (os_window_handle window, INT2 pos);
	OS_API bool set_window_title  (os_window_handle window, std::string title);
	OS_API bool move_window       (os_window_handle window, INT2 pos, INT2 size);
	OS_API bool destroy_window    (os_window_handle window);
	OS_API bool get_window_rect   (os_window_handle window, window_canvas_rect* rect);


	//inline BOOL set_my_console_title	(std::string title);
	//inline BOOL set_console_colors	(os_window_handle window, INT textc, INT buckc)	  ;
	//inline BOOL set_console_font		(os_window_handle window, wstring f, INT w, INT h);
}


LIB_NAMESPACE_END