#pragma once

#include "globals.h"

#if PLATFORM_OS == OS_WINDOWS
	#include "os.api.win.h"
#elif PLATFORM_OS == OS_UNIX
	#include "os.api.unix.h"
#elif
	#error "no os impl"
#endif
