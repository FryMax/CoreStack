#pragma once

#include "globals.h"

LIB_NAMESPACE_START

namespace os
{
	struct os_thread_id;
	struct os_handle;
	struct os_module;
	struct os_window_handle;
	struct os_window_event_msg;

	enum class ConsoleColor : short
	{
		Black        = 0,
		Blue         = 1,
		Green        = 2,
		Cyan         = 3,
		Red          = 4,
		Magenta      = 5,
		Brown        = 6,
		LightGray    = 7,
		DarkGray     = 8,
		LightBlue    = 9,
		LightGreen   = 0xA,
		LightCyan    = 0xB,
		LightRed     = 0xC,
		LightMagenta = 0xD,
		Yellow       = 0xE,
		White        = 0xF,
		_enum_size   = 0xE,
	};
	 
	enum class MKey : short
	{
 		MOUSE_LEFT    = 0x01,
		MOUSE_MIDDLE  = 0x02,
		MOUSE_RIGHT   = 0x03,
		MOUSE_FORWARD = 0x04,
		MOUSE_BUCK    = 0x05,
 		_enum_size,
	};
	enum class KKey : short
	{
 		KEY_LBUTTON  = 0x01, // L mouse button
		KEY_RBUTTON  = 0x02, // R mouse button
		KEY_CANCEL   = 0x03, // Control-break processing
		KEY_MBUTTON  = 0x04, // Middle mouse button
		KEY_XBUTTON1 = 0x05, // X1 mouse button
		KEY_XBUTTON2 = 0x06, // X2 mouse button

		KEY_BACK     = 0x08, // BACKSPACE
		KEY_TAB      = 0x09, // TAB
		KEY_CLEAR    = 0x0C, // CLEAR
		KEY_RETURN   = 0x0D, // ENTER
		KEY_SHIFT    = 0x10, // SHIFT
		KEY_CONTROL  = 0x11, // CTRL
		KEY_ALT      = 0x12, // ALT
		KEY_PAUSE    = 0x13, // PAUSE
		KEY_CAPITAL  = 0x14, // CAPS LOCK
		KEY_KANA     = 0x15, // IME Kana mode
		KEY_HANGUEL  = 0x15, // IME Hanguel mode
		KEY_HANGUL   = 0x15, // IME Hangul mode
		KEY_JUNJA    = 0x17, // IME Junja mode
		KEY_FINAL    = 0x18, // IME final mode
		KEY_HANJA    = 0x19, // IME Hanja mode
		KEY_KANJI    = 0x19, // IME Kanji mode

		KEY_ESCAPE   = 0x1B, // ESC
		KEY_SPACE    = 0x20, // Space
		KEY_PAGEUP   = 0x21, // PageUp
		KEY_PAGEDOWN = 0x22, // PageDown
		KEY_END      = 0x23, // End
		KEY_HOME     = 0x24, // Home
		KEY_LEFT     = 0x25, // Arrow left
		KEY_UP       = 0x26, // Arrow up
		KEY_RIGHT    = 0x27, // Arrow right
		KEY_DOWN     = 0x28, // Arrow down
		KEY_SELECT   = 0x29, // Select
		KEY_EXE      = 0x2B, // Execute key
		KEY_SNAPSHOT = 0x2C, // Snapshot
		KEY_INSERT   = 0x2D, // Insert
		KEY_DELETE   = 0x2E, // Delete
		KEY_HELP     = 0x2F, // Help

		KEY_0 = 0x30,
		KEY_1 = 0x31,
		KEY_2 = 0x32,
		KEY_3 = 0x33,
		KEY_4 = 0x34,
		KEY_5 = 0x35,
		KEY_6 = 0x36,
		KEY_7 = 0x37,
		KEY_8 = 0x38,
		KEY_9 = 0x39,

		KEY_A = 0x41,
		KEY_B = 0x42,
		KEY_C = 0x43,
		KEY_D = 0x44,
		KEY_E = 0x45,
		KEY_F = 0x46,
		KEY_G = 0x47,
		KEY_H = 0x48,
		KEY_I = 0x49,
		KEY_J = 0x4A,
		KEY_K = 0x4B,
		KEY_L = 0x4C,
		KEY_M = 0x4D,
		KEY_N = 0x4E,
		KEY_O = 0x4F,
		KEY_P = 0x50,
		KEY_Q = 0x51,
		KEY_R = 0x52,
		KEY_S = 0x53,
		KEY_T = 0x54,
		KEY_U = 0x55,
		KEY_V = 0x56,
		KEY_W = 0x57,
		KEY_X = 0x58,
		KEY_Y = 0x59,
		KEY_Z = 0x5A,

		KEY_WINLEFT  = 0x5B,  //  Left Windows key  (Natural keyboard)
		KEY_WINRIGHT = 0x5C,  //  Right Windows key (Natural keyboard)
		KEY_APPS     = 0x5D,  //  Applications key  (Natural keyboard)

		//[94-96] Reserved

		KEY_NUMPAD0 = 0x60,
		KEY_NUMPAD1 = 0x61,
		KEY_NUMPAD2 = 0x62,
		KEY_NUMPAD3 = 0x63,
		KEY_NUMPAD4 = 0x64,
		KEY_NUMPAD5 = 0x65,
		KEY_NUMPAD6 = 0x66,
		KEY_NUMPAD7 = 0x67,
		KEY_NUMPAD8 = 0x68,
		KEY_NUMPAD9 = 0x69,
		
		KEY_MULTIPLY  = 0x6A, // *
		KEY_ADD       = 0x6B, // +
		KEY_SEPARATOR = 0x6C, // /
		KEY_SUBTRACT  = 0x6D, // -
		KEY_DECIMAL   = 0x6E, // .
		KEY_DIVIDE    = 0x6F, // /
		
		KEY_F1  = 0x70,
		KEY_F2  = 0x71,
		KEY_F3  = 0x72,
		KEY_F4  = 0x73,
		KEY_F5  = 0x74,
		KEY_F6  = 0x75,
		KEY_F7  = 0x76,
		KEY_F8  = 0x77,
		KEY_F9  = 0x78,
		KEY_F10 = 0x79,
		KEY_F11 = 0x7A,
		KEY_F12 = 0x7B,
		KEY_F13 = 0x7C,
		KEY_F14 = 0x7D,
		KEY_F15 = 0x7E,
		KEY_F16 = 0x7F,
		KEY_F17 = 0x80,
		KEY_F18 = 0x81,
		KEY_F19 = 0x82,
		KEY_F20 = 0x83,
		KEY_F21 = 0x84,
		KEY_F22 = 0x85,
		KEY_F23 = 0x86,
		KEY_F24 = 0x87,

		KEY_NUMLOCK = 0x90, // NumLock
		KEY_SCROLL  = 0x91, // ScrLock

		KEY_LSHIFT   = 0xA0,
		KEY_RSHIFT   = 0xA1,
		KEY_LCONTROL = 0xA2,
		KEY_RCONTROL = 0xA3,
		KEY_LALT     = 0xA4,
		KEY_RALT     = 0xA5,

		KEY_PLUS     = 0xBB,  // +
		KEY_COMMA    = 0xBC,  // ,
		KEY_MINUS    = 0xBD,  // -
		KEY_PERIOD   = 0xBE,  // .
		KEY_TILDA    = 0xC0,  // ~
		KEY_EXPONENT = 0xDC,  // ^

		KEY_ATTN     = 0xF6,
		KEY_CRSEL    = 0xF7,
		KEY_EXSEL    = 0xF8,
		KEY_EREOF    = 0xF9,
		KEY_PLAY     = 0xFA,
		KEY_ZOOM     = 0xFB,
		KEY_NONAME   = 0xFC,
		KEY_PA1      = 0xFD,
		KEY_OEMCLEAR = 0xFE,

 		_enum_size = 256,
	};
	 
	struct window_canvas_rect
	{
		int loc_x;       // always 0
		int loc_y;       // always 0
		int loc_w;       // workspace wight
		int loc_h;       // workspace height
		int glob_left;   // workspace coord in system coords
		int glob_top;    // workspace coord in system coords
		int glob_right;  // workspace coord in system coords
		int glob_bot;    // workspace coord in system coords
	};
	 
	enum class window_z_depth
	{
		Z_TOP,
		Z_BOTTOM,
		Z_TOPMOST,
		Z_NORMAL,
	};
 
	enum class os_sys_metric : size_t
	{
		VISIBLE_MONITOR_COUNT,
		MAIN_MONITOR_W,
		MAIN_MONITOR_H,
	};
  
	enum class os_ram_metric : size_t
	{
 		MEM_MemoryLoadProcent,

		MEM_PageFileSize,
		MEM_PageFileLoad,
		MEM_PageFileAvail,

		MEM_VirtualMemoryTotal,
		MEM_VirtualMemoryLoad,
		MEM_VirtualMemoryAvail,

		MEM_RealMemoryLoad,
		MEM_RealMemoryTotal,
		MEM_RealMemoryAvail,

		MEM_CurentProcVirtualMemUsage,
		MEM_CurentProcRealMemUsage,
  	};

	enum class os_cpu_metric : size_t
	{
 		CPU_CpuLoadProcent,
		CPU_CpuNodeCount,
		CPU_CpuCoreCount,
		CPU_CpuThreadCount,
	};

}

LIB_NAMESPACE_END
