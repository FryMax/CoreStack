// todo:
// delete this file

#pragma once

enum class Platform {
	undefined,
	windows,
	linux,
	unix,
	android,
	ios,
	mac,
	freebsd,
};

enum class Arch {
	undefined,
	x32,
	x64,
	arm32,
	arm64,
};

enum class BuildMode {
	undefined,
	debug,
	release,
};

enum class GpuApi {
	undefined,
	software,
	opengl_2,
	opengl_3,
	opengl_es,
	//dx11
	//dx12
	//web_gl
	//volcan
};

////////////////////////////////////////////////////////////////////// platform

constexpr Platform PLATFORM =
#if defined(_WIN32) || defined(_WIN64)
	Platform::windows;
#elif defined(unix) || defined(__unix) || defined(__unix__)
	Platform::unix;
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if (TARGET_IPHONE_SIMULATOR == 1) || (TARGET_OS_IPHONE == 1)
		Platform::ios;
	#elif (TARGET_OS_MAC == 1)
		Platform::mac;
	#endif
#elif defined(ANDROID_MACRO_HERE)
	//todo
	#error(ANDROID_NOT_DEFINED)
#elif defined(__linux__) || defined(linux) || defined(__linux)
	Platform::linux;
#elif defined(__FreeBSD__)
	Platform::freebsd;
#else
	Platform::undefined;
#endif

////////////////////////////////////////////////////////////////////// arch

constexpr Arch ARCH =
#if defined(_WIN32) || defined(_WIN64)
	#if _WIN64
		Arch::x64;
	#else
		Arch::x32;
	#endif
#elif defined(__GNUC__)
	#if defined(__x86_64__) || defined(__ppc64__)
		Arch::x64;
	#else
		Arch::x32;
	#endif
#else
	#error(Bad arch)
#endif

// gnu compiler


////////////////////////////////////////////////////////////////////// buildmode

constexpr BuildMode BUILD_MODE =
#if defined(_DEBUG) || !defined(NDEBUG)
	BuildMode::debug;
#else
	BuildMode::release;
#endif
