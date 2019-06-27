#pragma once

#include "globals.h"
#include "os.types.h"

LIB_NAMESPACE_START

namespace os
{
	struct os_thread_id     { };
	struct os_handle        { };
	struct os_window_handle { };
	struct os_module        { };

	struct os_window_event_msg
	{
		os_window_event_msg() {}

	protected:
		uint8_t padding[256] = { 0 };
	};

}

LIB_NAMESPACE_END