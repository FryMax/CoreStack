#pragma once

//==================================================| OS
									//              |
#define OS_WINDOWS         0xAA     //              |   Windows
#define OS_LINUX           0xBA     // no impl      |   Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos
#define OS_UNIX            0xBB     // no impl      |   Unix like OS
#define OS_ANDROID         0xBC     // no impl      |   Android (implies Linux, so it must be searched first)
#define OS_IOS             0xCA     // no impl      |   Apple ios
#define OS_MAC             0xCB     // no impl      |   Apple OSX
#define OS_FREEBSD         0xDA     // no impl      |   FreeBSD, NetBSD, OpenBSD, DragonFly BSD
									//              |
//==================================================| BUILD TARGET ARCH
									//              |
#define ARCH_X32           0xAA     //              |   x32, x86,
#define ARCH_X64           0xAB     //              |   x64, x86-64, AMD64, Intel64, EM64T
#define ARCH_ARM32         0xBA     // no impl      |   ARMv1 - ARMv8-A
#define ARCH_ARM64         0xBB     // no impl      |   ARMv8-A - ARMv8.4-A
									//              |
//==================================================| BUILD MODE
									//              |
#define BUILD_MODE_DEBUG   0x01     //              |
#define BUILD_MODE_RELEASE 0x02     //              |
									//              |
//==================================================| GPU API IMPL (mb dell)
									//              |
#define __gpu_api_software  0xAA    //              |
#define __gpu_api_dx_11     0xBA    //              |
#define __gpu_api_dx_12     0xBB    // no impl      |
#define __gpu_api_opengl_2  0xCA    //              |
#define __gpu_api_opengl_3  0xCB    //              |
#define __gpu_api_opengl_ES 0xCC    //              |
#define __gpu_api_web_gl    0xCD    // no impl      |
#define __gpu_api_volcan    0xDA    // no impl      |
									//              |
//==================================================|

#if defined(_WIN32) || defined(_WIN64)
	#define INTERNAL_PLT_OS OS_WINDOWS
#elif defined(unix)	|| defined(__unix) || defined(__unix__)
	#define INTERNAL_PLT_OS OS_UNIX
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#define INTERNAL_PLT_OS OS_IOS
	#elif TARGET_OS_IPHONE == 1
		#define INTERNAL_PLT_OS OS_IOS
	#elif TARGET_OS_MAC == 1
		#define INTERNAL_PLT_OS OS_MAC
	#endif
#elif defined(ANDROID_DEF_HERE)

	#error(ANDROID?)

#elif defined(__linux__) || defined(linux) || defined(__linux)
	#define INTERNAL_PLT_OS OS_LINUX
#elif defined(__FreeBSD__)
	#define INTERNAL_PLT_OS OS_FREEBSD
#else
	#error (PLATFORM?)
#endif

////////////////////////////////////////////////////////////////////// arch

// vs compiler
#if defined(_WIN32) || defined(_WIN64)
	#if _WIN64
		#define INTERNAL_PLT_ARCH ARCH_X64
	#else
		#define INTERNAL_PLT_ARCH ARCH_X32
	#endif
#endif

// gnu compiler
#if defined(__GNUC__)
	#if defined(__x86_64__) || defined(__ppc64__)
		#define INTERNAL_PLT_ARCH ARCH_X64
	#else
		#define INTERNAL_PLT_ARCH ARCH_X32
	#endif
#endif

////////////////////////////////////////////////////////////////////// build mode

#if defined(_DEBUG) || !defined(NDEBUG)
	#define INTERNAL_BUILD_MODE BUILD_MODE_DEBUG
#else
	#define INTERNAL_BUILD_MODE BUILD_MODE_RELEASE
#endif

////////////////////////////////////////////////////////////////////// gpu api

//#define __build_ver_str "0.0.0.0"
#define __version(major,minor) (20 << (long)major | (long)minor)

#define internal_gpu_api __gpu_api_opengl_2

#if (internal_gpu_api == __gpu_api_software)
	#define __gpu_api_ver __version(0,0)
#elif (internal_gpu_api == __gpu_api_dx_11)
	#define __gpu_api_ver __version(11,0)
#elif(internal_gpu_api == __gpu_api_dx_12)
	#define __gpu_api_ver __version(12,0)
#elif(internal_gpu_api == __gpu_api_opengl_2)
	#define __gpu_api_ver __version(2,0)
#elif (internal_gpu_api == __gpu_api_opengl_3)
	#define __gpu_api_ver __version(3,3)
#elif (internal_gpu_api == __gpu_api_volcan)
	#define __gpu_api_ver __version(0,0)
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PLATFORM_OS     INTERNAL_PLT_OS
#define PLATFORM_ARCH   INTERNAL_PLT_ARCH
#define BUILD_MODE      INTERNAL_BUILD_MODE
#define BUILD_TIMESTAMP __TIMESTAMP__

// switch / mbdell

#define GPU_API       internal_gpu_api
#define FAKE_EM_BUILD (false)

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FAKE_EM_BUILD
	#define FAKE_EM_BUILD (false)
#endif

#ifdef __EMSCRIPTEN__
	#define EM_BUILD (true)
#else
	#define EM_BUILD (false)
#endif

#if (EM_BUILD || FAKE_EM_BUILD)

//  #undef  PLATFORM_OS
//  #define PLATFORM_OS      __os_unix
	#undef  GPU_API
	#define GPU_API          __gpu_api_opengl_ES

	#define EXTERN_LOGGER   (false)
	#define FORCE_SDL_USAGE (true)
	#define BUILD_LIGHT		(true)

#else

	#define EXTERN_LOGGER   (true)
	#define FORCE_SDL_USAGE (false)
	#define BUILD_LIGHT		(false)

#endif
//=============================================================================// extra flags

#if (PLATFORM_OS == OS_WINDOWS)

	#define GFLAGS_WINDOWS_OPT_BUILD (true)   // windows optimized build
	#define GFLAGS_WINDOWS_CMP_MODE  (false)  // old systems compatibility

	#if (GFLAGS_WINDOWS_OPT_BUILD)
		#define UNICIDE                         // for <windows.h>
		#define STRICT                          // for <windows.h>
		#define NOMINMAX                        // for <windows.h>
		#define	VC_EXTRALEAN                    // Exclude unnecessary headers MFC
		#define WIN32_LEAN_AND_MEAN             // Exclude unnecessary headers Windows.h
		#define _CRT_SECURE_NO_WARNINGS         // Skip security check CRT
		#define _WINSOCK_DEPRECATED_NO_WARNINGS // Skip security check WINSOCK2
	#endif

	#if (GFLAGS_WINDOWS_CMP_MODE)
		#define WINVER       0x0501 // _WIN32_WINNT_WINXP
		#define _WIN32_WINNT 0x0501 // _WIN32_WINNT_WINXP
	#endif

#endif

//=============================================================================// break helpers

// TODO:
// need testing
// usage: psnip_trap();
#include <3rd-party/debug-trap.h>

namespace
{
	//#pragma optimize( "", off )
	//inline volatile void MAKE_FAKE_EXCEPION() { volatile int __FEXP = *(volatile int*)0; (void)__FEXP; }
	//#pragma optimize( "", on )
}

#define MAKE_FAKE_EXCEPION() { volatile int __FEXP = *(volatile int*)0; (void)__FEXP; }

#define INTERNAL_LOG_BREAK_LOCATION(     ){log::error << (std::string(__FILE__) + ":" + std::to_string(__LINE__)+ " (" + std::string(__FUNCTION__) + "): (breakpoint)");}
#define INTERNAL_LOG_BREAK_LOCATION_STR(s){log::error << (std::string(__FILE__) + ":" + std::to_string(__LINE__)+ " (" + std::string(__FUNCTION__) + "]: " + std::string(s));}
#if (BUILD_MODE == BUILD_MODE_DEBUG)
	#define INTERNAL_ADD_DEBUG_CODE(x) x
	#define INTERNAL_BREAK_PROGRAM(){psnip_trap();}
#else
	#define INTERNAL_ADD_DEBUG_CODE(x)
	#define INTERNAL_BREAK_PROGRAM(){MAKE_FAKE_EXCEPION();}
#endif

//=============================================================================// macro

// Add code for debug build only
#define DEBUG_CODE(x) {INTERNAL_ADD_DEBUG_CODE(x)}

// Stop program immediately
#define BREAK(            ) {INTERNAL_LOG_BREAK_LOCATION();INTERNAL_BREAK_PROGRAM(); throw std::runtime_error("");}
#define BREAK_IF(      val) {if(val) BREAK();}
#define DEBUG_BREAK(      ) {DEBUG_CODE(        BREAK();)}
#define DEBUG_BREAK_IF(val) {DEBUG_CODE(if(val) BREAK();)}

// Error handling (need test)
#define FUSE_IF(val,s,act)  {if(val){INTERNAL_LOG_BREAK_LOCATION_STR(s); act;}}

//=============================================================================// std

//// TODO:
////  delete this
/**/
/**/ #undef  _HAS_STD_BYTE
/**/ #define _HAS_STD_BYTE 0
/**/ #undef  _HAS_STD_BOOLEAN
/**/ #define _HAS_STD_BOOLEAN 0
/**/ #pragma warning(disable : 4984) // "if constexpr" is c++17
/**/ #pragma warning(disable : 6326) // constant in if/switch
/**/ using byte = unsigned char;

/**/ // using u8  = uint8_t;
/**/ // using u16 = uint16_t;
/**/ // using u32 = uint32_t;
/**/ // using u64 = uint64_t;
/**/ // using i8  = int8_t;
/**/ // using i16 = int16_t;
/**/ // using i32 = int32_t;
/**/ // using i64 = int64_t;
/**/ // using f32 = float;
/**/ // using f64 = double;

#include "std_all.h"

//=============================================================================//

inline auto GetBuildInfo()
{
	constexpr static char* UNDEFINDED = "{null}";

	auto get_platform_os = []() -> std::string
	{
		switch (PLATFORM_OS)
		{
		break; case OS_WINDOWS : return "Windows";
		break; case OS_LINUX   : return "Linux";
		break; case OS_UNIX    : return "Unix";
		break; case OS_ANDROID : return "Android";
		break; case OS_IOS     : return "Ios";
		break; case OS_MAC     : return "Mac";
		break; case OS_FREEBSD : return "FreeBsd";
		break; default         : return UNDEFINDED;
 		}
	};
	auto get_extra_options = []() -> std::string
	{
		std::string out;
		#if(PLATFORM_OS == OS_WINDOWS)
			if (GFLAGS_WINDOWS_OPT_BUILD) out += "-WinMin ";
			if (GFLAGS_WINDOWS_CMP_MODE)  out += "-WinXP ";
		#endif

		if (EM_BUILD)      out += "-EmBuild ";
		if (FAKE_EM_BUILD) out += "-FakeEmBuild ";

		return out;
	};
	auto get_patform_arch = []() -> std::string
	{
		switch (PLATFORM_ARCH)
		{
		break; case ARCH_X32   : return "x32";
		break; case ARCH_X64   : return "x64";
		break; case ARCH_ARM32 : return "ARMx32";
		break; case ARCH_ARM64 : return "ARMx64";
		break; default                : return UNDEFINDED;
		}
	};
	auto get_gpu_api = []() -> std::string
	{
		switch (GPU_API)
		{
		break; case __gpu_api_software  : return "Software";
		break; case __gpu_api_dx_11     : return "DX-11";
		break; case __gpu_api_dx_12     : return "DX-12";
		break; case __gpu_api_opengl_2  : return "GL-2";
		break; case __gpu_api_opengl_3  : return "GL-3";
		break; case __gpu_api_opengl_ES : return "GL-ES";
		break; case __gpu_api_volcan    : return "Volcan";
		break; default                  : return UNDEFINDED;
		}
	};
	auto get_build_mode = []() -> std::string
	{
		switch (BUILD_MODE)
		{
		break; case BUILD_MODE_DEBUG   : return "debug";
		break; case BUILD_MODE_RELEASE : return "release";
		break; default:                  return UNDEFINDED;
		}
	};
	auto get_byte_order = []() -> std::string
	{
		// todo find better constexpr solution or wait c++20?
		volatile constexpr uint16_t x = '\1';
		volatile const bool isBigEndian = *reinterpret_cast<volatile const uint8_t*>(&x) == 0;
		return isBigEndian ? "big endian" : "little endian";
	};
	auto get_build_timestamp = []() -> std::string
	{
		return std::string(BUILD_TIMESTAMP);
	};
	//------------------------------------------------------------------------------------------------
	using build_meta = std::pair<std::string, std::string>;
	std::vector<build_meta> meta;

	meta.emplace_back(build_meta{ "arch",       get_patform_arch   () });
	meta.emplace_back(build_meta{ "platform",   get_platform_os    () });
	meta.emplace_back(build_meta{ "options",    get_extra_options  () });
	meta.emplace_back(build_meta{ "build mode", get_build_mode     () });
	meta.emplace_back(build_meta{ "build time", get_build_timestamp() });
	meta.emplace_back(build_meta{ "byte order", get_byte_order     () });
	meta.emplace_back(build_meta{ "gpu api",    get_gpu_api        () });

	return meta;
}

//=============================================================================//
template <typename T, size_t N> constexpr uint8_t(&INTERNAL_ARRAY_SIZE_HELPER(T(&array)[N]))[N] {};

// mb dell

//#define __f   inline
//#define __ff  __forceinline
//#define __ef  constexpr inline
//#define __cf  constexpr inline

#define internal_array_size(x) (sizeof(INTERNAL_ARRAY_SIZE_HELPER(x)))

#define internal_call_once(x)             static bool             ico_##x     = false; if (!std::exchange(ico_##x, true))
#define internal_call_once_atom(x)        static std::atomic_bool ico_atom##x = false; if (!ico_atom##x.exchange (true))
#define internal_call_once_wait(x,code) { static int ico_wait##x = [&](){ code ; return 0;}(); }

#define internal_make_once(x)             internal_call_once(x)
#define internal_make_once_atom(x)        internal_call_once_atom(x)
#define internal_make_once_wait(x, code)  internal_call_once_wait(x, code)

#define internal_for_from(i,j,size)       for(size_t i = static_cast<size_t>(j); i < static_cast<size_t>(size); i++) //

#if defined(__GNUC__)
    #define internal_aligned_var_placement(al)  __attribute__((aligned(N)))
#else
    #define internal_aligned_var_placement(al)  __declspec(align(al))
#endif
//=============================================================================// Helpers

#define ARRAY_SIZE(array)       internal_array_size(array)          // array size helper
#define ALIGNED(al)             internal_aligned_var_placement(al)  // stack vabs alignment

#define MakeOnce                internal_make_once(__LINE__)            // threadsafe(-) | wait(-) // [deprecated]
#define MakeOnceAtom            internal_make_once_atom(__LINE__)       // threadsafe(+) | wait(-) // [deprecated]
#define MakeOnceWait(code)      internal_make_once_wait(__LINE__, code) // threadsafe(+) | wait(+) // [deprecated]
#define MAKE_ONCE(code)         internal_make_once_wait(__LINE__, code) // threadsafe(+) | wait(+)

#define ForArray(i,size)        internal_for_from(i,0,size) // [deprecated]
#define ForArrayFrom(i,j,size)  internal_for_from(i,j,size) // [deprecated]

// TODO:
//  MB DEL
//
// #define DELETE(p)              { if(p){                delete  (p); (p)=nullptr;} } // [deprecated]
// #define DELETE_ARRAY(p)        { if(p){                delete[](p); (p)=nullptr;} } // [deprecated]
// #define CLOSE(p)               { if(p){(p)->Close  (); delete  (p); (p)=nullptr;} } // [deprecated]
// #define RELEASE(p)             { if(p){(p)->Release();              (p)=nullptr;} } // [deprecated]

//=============================================================================// Lit

// Note:
//   wchar_t is 2 bytes on Windows, otherwise 4 bytes

#define IN_CHAR(  X)      X  // const char[]      |  Narrow multibyte string literal          |  Recommended for native use
#define IN_WCHAR( X)   L##X  // const wchar_t[]   |  Wide string literal                      |  Not recomended
#define IN_UTF8(  X)  u8##X  // const char[]      |  Unicode UTF - 8  encoded string literal  |  Recommended for interaction
#define IN_UTF16( X)   u##X  // const char16_t[]  |  Unicode UTF - 16 encoded string literal  |  Not recomended
#define IN_UTF32( X)   U##X  // const char32_t[]  |  Unicode UTF - 32 encoded string literal  |  Not recomended
#define IN_RAW(   X)    R#X  //                   |  Raw string literal
#define IN_QUOTES(X)     #X  //                   |  Put sentence in quotes


// TODO:
//  MB DELL
#define LOG_WARN_NO_IMPL log::warn << std::string(__FUNCTION__) + ": (no impl)";

class NonAssignable
{
public:
	NonAssignable(NonAssignable const&) = delete;
	NonAssignable& operator=(NonAssignable const&) = delete;
	NonAssignable() {}
};

inline void* _allocate_aligned(void* ptr, size_t size, size_t align) //noexcept
{
	//FUSE_IF(align == 0, "unexpected zero aligned alloc", return nullptr);

	if (align == 0)
	{
		//return nullptr;
		throw std::invalid_argument("unexpected zero alignment");
	}

	const size_t buffer_size = size + align + sizeof(size_t);
	const size_t buffer_iprt = reinterpret_cast<size_t>(std::malloc(buffer_size));

	//FUSE_IF(buffer_iprt == 0, "bad aligned alloc", return nullptr);

	if (buffer_iprt == 0)
	{
		//return nullptr;
		throw std::bad_alloc();
	}

	const size_t end_iptr = buffer_iprt + buffer_size;
	const size_t dat_iptr = (end_iptr - size) - (end_iptr - size) % align;

	ptr = reinterpret_cast<size_t*>(dat_iptr);
	*reinterpret_cast<size_t*>(dat_iptr - sizeof(size_t)) = buffer_iprt;

	return ptr;
}

inline void _free_aligned(void *ptr)
{
	if (!ptr)
		throw std::invalid_argument("nullpointer is unexpected");

    size_t iptr = reinterpret_cast<size_t>(ptr) - sizeof(size_t);
    size_t ival = *reinterpret_cast<size_t*>(iptr);
    std::free(reinterpret_cast<void*>(ival));
    ptr = nullptr;
}


namespace
{
	template<typename T>
	void pass(T) {};

	template<typename T>
	int some_function(T) {};

	template<typename... Args>
	inline void expand(Args&&... args)
	{
		pass(some_function(args)...);
	}

	template<typename T>
	struct function_traits;

	template<typename R, typename Cl, typename ...Args>
	struct function_traits<std::function<R(Cl, Args...)>>
	{
		static const size_t nargs = sizeof...(Args);

		typedef R result_type;

		template <size_t i>
		struct arg
		{
			typedef typename std::tuple_element<i, std::tuple<Cl, Args...>>::type type;
		};
	};
}
//=============================================================================//
#define LIB_NAME			corelib
#define LIB_NAMESPACE_START namespace corelib {
#define LIB_NAMESPACE_END	}

namespace corelib
{
    //using byte   = std::uint8_t;
	using byte_t = std::uint8_t;

	namespace log
	{
		//using wchar  = std::uint8_t;
		//namespace log    {};
		//namespace str    {};
		//namespace math   {};
		//namespace art    {};
		//namespace os     {};

		struct logger
		{
			void(*link)(const std::string&) = nullptr;
		};

		extern logger error; //
		extern logger warn;  //
		extern logger info;  //
		extern logger debug; //
		extern logger trace; //

		bool update_output_links();
		logger& operator<<(logger& llog, const std::string& text);

	}

	namespace utka
	{
		struct u_virt_type {};
		struct u_type_int32 : u_virt_type {};
		struct u_type_float : u_virt_type {};

		enum class uval_type
		{

			//------------ args?
			u_int32,
			u_float,
			u_bool,
			u_cstr,  // std::reference_wrapper<const std::string>

			//------------ props?

			u_int8,
			u_int16,
			u_int64,

			u_str_ptr,

			u_unknown,
		};

		enum class uprop_access
		{
			read,
			read_write,
		};

		struct uarg
		{
		public:
			using cstr = const std::string&;

			uarg(int   e)              : int32   (e), type(uval_type::u_int32) {}
			uarg(float e)              : real    (e), type(uval_type::u_float) {}
			uarg(bool  e)              : boolean (e), type(uval_type::u_bool ) {}
			uarg(const std::string& e) : str     (e), type(uval_type::u_cstr ) {}

			int   get_int32() const { BREAK_IF(type != uval_type::u_int32); return int32;   }
			float get_float() const { BREAK_IF(type != uval_type::u_float); return real;    }
			bool  get_bool () const { BREAK_IF(type != uval_type::u_bool ); return boolean; }
			cstr  get_srt  () const { BREAK_IF(type != uval_type::u_cstr ); return str;     }

		private:
			union
			{
				const int   int32;
				const float real;
				const bool  boolean;
				const std::reference_wrapper<const std::string> str;
			};
			const uval_type type;
		};
		//---------------------------------------------------------------------
		struct uclass_meta
		{
			//std::string obj_id; //
			std::string obj_type; // type_id name
			std::string obj_name; // given name
			std::string obj_func; // src func
			std::string obj_file; // src file
			std::string obj_line; // src line
		};
		struct uprop_meta
		{
			const std::string  alias;
			const uval_type    type;
			const uprop_access access;
			void* const        val_ptr;
		};
		struct ufunc_meta
		{
			struct uarg_item
			{
				std::string name;
				uval_type   type;
			};

			using uargs_t = std::vector<uarg_item>;
			using ufunc_t = std::function<void(const std::vector<uarg>&)>;

			const std::string alias;
			const uargs_t arg_types;
			const ufunc_t func;
		};

		struct uclass_pin
		{
			//todo store pids separately
			std::string id;//aka pid
			uclass_meta meta;
			std::vector<uprop_meta> props;
			std::vector<ufunc_meta> funcs;
		};
		//---------------------------------------------------------------------
		class uclass
		{
		public:
			uclass(uclass&&) = delete;
			uclass(const uclass&&) = delete;
			uclass& operator=(uclass&&) = delete;
			uclass& operator=(const uclass&&) = delete;

		public:
			uclass(
				const char* name,
				const char* type,
				const char* func,
				const char* file,
				int line);

			virtual ~uclass();

			template<typename T>
			void _register_prop(T* const ptr, const char* alias, uprop_access access = uprop_access::read_write);

			template<typename T = void>
			void _register_func(const char* alias, ufunc_meta::uargs_t&& args, const ufunc_meta::ufunc_t& func);

		};

		#define UCLASS(tag) utka::uclass(tag, typeid(this).name(), __FUNCTION__, __FILE__, __LINE__)
		//---------------------------------------------------------------------
		using upid = std::string;

		class umanager_data
		{
		protected:
			std::vector<uclass_pin> m_pins;
		};

		class umanager_class : umanager_data, uclass
		{
			friend class uclass;

		private:

			//bool is_upid_free(const upid& upid)
			//{
			//	return m_pins.end() == std::find_if(m_pins.begin(), m_pins.end(),
			//		[upid](const decltype(m_pins)::value_type& e)
			//		{
			//			return upid == e.id;
			//		});
			//}

			struct pinout
			{
				const bool finded;
				uclass_pin& pin;
			};

			pinout find_pin(const upid& upid)
			{
				decltype(m_pins)::iterator it = std::find_if(m_pins.begin(), m_pins.end(),
					[upid](const decltype(m_pins)::value_type& e)
					{
						return upid == e.id;
					});

				static uclass_pin default_pin;

				return it != m_pins.end() ?
					pinout{ true, *it } :
					pinout{ false, default_pin };
			}

		private:

			int32_t     test_value_int32;
			float       test_value_flt;
			bool        test_value_real;
			std::string test_value_str;


			int32_t test_func(int arg) { log::warn << __FUNCTION__ + std::to_string(arg); return 0; }

		public:
			umanager_class() : umanager_data(), UCLASS("umanager")
			{
				_register_prop(&test_value_int32, "integer", uprop_access::read_write);
				_register_prop(&test_value_flt,	  "FLOAT",   uprop_access::read_write);
				_register_prop(&test_value_flt,   "FLOAT",   uprop_access::read);
				_register_prop(&test_value_real,  "boolean", uprop_access::read_write);
				_register_prop(&test_value_real,  "boolean", uprop_access::read);
				_register_prop(&test_value_str,   "str ptr", uprop_access::read_write);

				_register_func("test function",
					{
						{"IntArg", uval_type::u_int32}
					},
					[this](const std::vector<uarg>& args)
					{
						this->test_func(args[0].get_int32());
					});
			}
			//-----------------------------------------------------------------

			const std::vector<uclass_pin>& get_meta_array() const
			{
				return m_pins;
			}

		private:
			void release_uclass(const upid& id)
			{
				//LOG_WARN_NO_IMPL;
				//release props
				//release funcs
				//release class
			}
			void register_uclass(const upid& id, uclass_meta&& inst)
			{
				BREAK_IF(id.empty());
				BREAK_IF(inst.obj_name.empty());
				BREAK_IF(inst.obj_type.empty());
				BREAK_IF(inst.obj_func.empty());
				BREAK_IF(inst.obj_file.empty());
				BREAK_IF(inst.obj_line.empty());
				BREAK_IF(find_pin(id).finded == true);

				//New uclass header
				uclass_pin pin;

				pin.id = id;
				pin.meta = std::move(inst);
				pin.props.shrink_to_fit();
				pin.funcs.shrink_to_fit();

				m_pins.push_back(std::move(pin));
			}

			void release_prop(const upid& id, void* prop_ptr) { LOG_WARN_NO_IMPL; }
			void register_prop(const upid& id, uprop_meta&& prop)
			{
				BREAK_IF(id.empty());
				BREAK_IF(prop.alias.empty());
				BREAK_IF(prop.val_ptr == nullptr);

				auto uobj = find_pin(id);
				BREAK_IF(uobj.finded == false);

				const auto& pvec = uobj.pin.props;
				const auto& it = std::find_if(pvec.begin(), pvec.end(),
					[prop](const decltype(uobj.pin.props)::value_type& e)
					{
						return prop.val_ptr == e.val_ptr;
					});
				//todo uncomm
				//BREAK_IF(it != pvec.end()); //value ptr allready registered?

				uobj.pin.props.push_back(std::move(prop));
			}

			void release_func(const upid& id) { LOG_WARN_NO_IMPL; }
			void register_func(const upid& id, ufunc_meta&& func)
			{
				BREAK_IF(id.empty());
				BREAK_IF(func.alias.empty());

				auto uobj = find_pin(id);
				BREAK_IF(uobj.finded == false);

				const auto& fvec = uobj.pin.funcs;
				const auto& it = std::find_if(fvec.begin(), fvec.end(),
					[func](const decltype(uobj.pin.funcs)::value_type& e)
					{
						return func.func.target_type() == e.func.target_type();
					});
				BREAK_IF(it != fvec.end());

				uobj.pin.funcs.push_back(std::move(func));
			}

		};

		extern umanager_class UManagerClass;

		namespace
		{
			template<typename T>
			constexpr uval_type get_value_type(T* const ptr)
			{
				if constexpr (std::is_same_v<int8_t,  T>) { return uval_type::u_int8;    }
				if constexpr (std::is_same_v<int16_t, T>) { return uval_type::u_int16;   }
				if constexpr (std::is_same_v<int32_t, T>) { return uval_type::u_int32;   }
				if constexpr (std::is_same_v<int64_t, T>) { return uval_type::u_int64;   }
				if constexpr (std::is_same_v<float,   T>) { return uval_type::u_float;   }
				if constexpr (std::is_same_v<double,  T>) { return uval_type::u_unknown; }
				if constexpr (std::is_same_v<bool,    T>) { return uval_type::u_bool;    }
				if constexpr (std::is_same_v<std::string, T>) { return uval_type::u_str_ptr; }
				return uval_type::u_unknown;
			}
		}

		template<typename T>
		inline void uclass::_register_prop(T* const ptr, const char* alias, uprop_access access)
		{
			BREAK_IF(ptr == nullptr);
			BREAK_IF(alias == nullptr);

			const auto val_type = get_value_type(ptr);
			BREAK_IF(val_type == uval_type::u_unknown);

			UManagerClass.register_prop(std::to_string((size_t)this),
				uprop_meta
				{
					std::string(alias ? alias : ""),//todo
					val_type,
					access,
					(void* const)ptr,
				});
		}

		template<typename T>
		inline void uclass::_register_func(const char* alias, ufunc_meta::uargs_t&& args, const ufunc_meta::ufunc_t& func)
		{
			BREAK_IF(alias == nullptr);

			UManagerClass.register_func(std::to_string((size_t)this),
				ufunc_meta
				{
					std::string(alias),
					std::move(args),
					func,
				});
		}

};

//////////////////////////////////////////////////////////////////////////
LIB_NAMESPACE_END
using namespace LIB_NAME;

// TODO:
//	md dell, replace with std::optional
namespace operation_results
{
	struct RESULT
	{
		bool is_ok = false;

		inline RESULT() {}
		inline bool IsFailed (void) { return !is_ok; }
		inline bool IsSucceed(void) { return  is_ok; }
	};

	static inline RESULT& operator<<(RESULT& result, const bool arg)
	{
		result.is_ok = arg;
		return result;
	}

	//--------------------------------------------------------------------//

	enum class rerr_code : int
	{
		//0 -
		undefined = -1,
		unknown = 0,
		//...
		arg_error = 99,
		//...

		call_no_impl = 999,


		_memory_errors_range_start = 200,

		out_of_memory_error = 200,
		corrupted_memory_error = 202,

		_memory_errors_range_end = 300,

	};

	struct result_null_template {};

	// mb change to std::optional when will supported by em
	template<typename T = result_null_template>
	struct result : RESULT
	{
	public:
		result() = delete;
		//{
		//	is_ok = true;
		//}
		result(T arg)
		{
			is_ok = true;
			value = arg;
		}
		explicit result(T&& arg)
		{
			is_ok = true;
			value = arg;
		}
		////////////////////////////////////////////////////////////////////////// error result constructor
		result(const T& def_value, rerr_code error_code, const std::string& error_msg, int user_error_code = 0)
		{
			log::error << std::string("result error: ") + error_msg + " (REC/UEC: " + std::to_string((int)error_code) + "/" + std::to_string((int)user_error_code) + ")";

			value    = def_value;
			e_string = error_msg;
			e_code   = error_code;
			ue_code  = user_error_code;
		}
		result(rerr_code error_code, const std::string& error_msg)
		{
			log::error << std::string("result error: ") + error_msg + " (REC: " + std::to_string((int)error_code) + ")";

			e_code = error_code;
			e_string = error_msg;
		}
		///////////////////////////////////////////////////////////////////////////////////
		result(const result<result_null_template>& res)
		{
			e_string = res.GetErrorMsg();
			e_code   = res.GetErrorCode();
			ue_code  = res.GetErrorUserCode();
		}

	public:
		rerr_code   GetErrorCode     () const { return e_code;   }
		std::string GetErrorMsg      () const { return e_string; }
		int         GetErrorUserCode () const { return ue_code;  }

		T GetValue()
		{
			if (!is_ok)
				assert(false);

			return std::move(value);
		}

	protected:

		T value;

		int32_t		ue_code = 0;
		rerr_code	e_code = rerr_code::undefined;
		std::string e_string = "";
	};

	namespace
	{
		constexpr static rerr_code	_def_error_code = rerr_code::undefined;
		constexpr static const char	_def_error_string[] = "Error result message not provided";

	}

	static inline result<result_null_template> error_result(const std::string& error_msg, rerr_code error_code = _def_error_code)   { return result<result_null_template>(error_code, error_msg); }
	static inline result<result_null_template> error_result(rerr_code error_code, const std::string& error_msg = _def_error_string) { return result<result_null_template>(error_code, error_msg); }

	template<typename T>
	static inline result<T> error_result(const T& def_value, rerr_code error_code, const std::string& error_msg, int user_error_code = 0)
	{
		return result<T>(def_value, error_code, error_msg, user_error_code);
	}
	template<typename T>
	static inline result<T> error_result(const T& def_value, const std::string& error_msg, rerr_code error_code = rerr_code::undefined, int user_error_code = 0)
	{
		return result<T>(def_value, error_code, error_msg, user_error_code);
	}

	// Usage samples:
	static inline result<float> function_with_result_template()
	{
		//___________________Succeed result:________________

		return 63.0f;
		return 5;
		return "abc"[0];
		return 0.5;

		result<float> aa = 5.5;
	}

	static inline void result_function_call_template()
	{
		auto ok = function_with_result_template();

		if (ok.IsSucceed())
		{
			float value = ok.GetValue();
			(void)value;
		}
		else
		{
			std::cerr << ok.GetErrorMsg();
			std::cerr << (int)ok.GetErrorCode();
			std::cerr << ok.GetErrorUserCode();
		}
	}

};

using operation_results::RESULT;
using operation_results::result;
using operation_results::rerr_code;
using operation_results::error_result;

//////////////////////////////////////////////////////////////////////////

#define DEF_UMODULE(...)

#if(false)

namespace UNIVERSAL
{
	struct	UTAG;
	class	UCLASS;
	class	UMODULE;
	static	std::vector<UMODULE*> UMOD;
	//----------------------------------------------------------------------//
 	struct UTAG
	{
	public:
		UTAG() {}
		UTAG(int id)			  : my_mod_id(id), my_mod_ptr(nullptr)	{}
		UTAG(int id, UMODULE*ptr) : my_mod_id(id), my_mod_ptr(ptr)		{}
	public:
		int		 my_mod_id;	 //	array id
		UMODULE* my_mod_ptr; // may be nullptr or old value ptr
	};
	static void CheckMainModule();
	static UTAG GetNewUModuleTag();

	//////////////////////////////////////////////////////////////////////////
	enum class ex_module_information
	{
        name,
		code_name,
		version,
		release_date,
		description,
		license,
		author,
		link_git,
		link_offsite,
		ex_info,

		_enum_size,
 	};
	using emi = ex_module_information;
 	struct exdata
	{
		exdata(emi i, std::string txt)
		{
			id	 = i;
			text = txt;
		}
		emi			id;
		std::string	text;
	};
 	using minfo = std::vector<exdata>;
	//////////////////////////////////////////////////////////////////////////

	class UMODULE
	{
	public:
		UMODULE(std::string def_name,
				std::string def_vers,
				std::string def_desc = "No desctiption",
  				std::string def_file = 0,
				std::string def_line = 0)
		{
			CheckMainModule();
			//---------------------------------// add in array only if not def yet
 			ForArray(i, UMODULESARR.size())
			{
				//todo
				//exept(UMODULESARR[i]->u_mod_name == def_name); // file compilation sequence error
  			}
			UMODULESARR.push_back(this);
			//---------------------------------// fill data
			u_mod_name = def_name;
			u_mod_ver  = def_vers;
 			u_mod_file = "\"" + def_file.substr(def_file.rfind("\\") + 1, def_file.size()) + "\"";
			u_mod_line = def_line;
   		}

	public:
			std::string u_mod_name;
			std::string u_mod_ver;
			std::string u_mod_desc;
 			std::string u_mod_file;
			std::string u_mod_line;
 			std::vector<UCLASS*> my_refs; // array to ref classes
 	};
	//----------------------------------------------------------------------//
 	class UCLASS
	{
	public:
		//virtual ~UCLASS() { log::debug << "UCLASS destroyed. check this"; }

		UCLASS(	std::string def_name,
				//std::vector<UTAG> Modules = { UTAG(0, nullptr) },
				std::string def_file = 0,
				std::string def_line = 0)
		{
  			CheckMainModule();
			//---------------------------------// register this ref class in all included modules

			//ForArray(i, Modules.size())
			//{
			//	UMODULE* mod = Modules[i].my_mod_ptr;
 			//	if (mod != nullptr)
 			//	{
			//		mod->my_refs.push_back(this);
 			//	}
			//}

			//---------------------------------// fill data
  			u_c_name	= def_name;
			u_c_file	= "\"" + def_file.substr(def_file.rfind("\\") + 1, def_file.size()) + "\"";
			u_c_line	= def_line;
			//u_c_modules = Modules;
 		}

	public:
 		std::string			u_c_name;
		std::string			u_c_file;
		std::string			u_c_line;
 		//std::vector<UTAG>	u_c_modules;
	};

	static void CheckMainModule()
	{
		MakeOnce
		{
			if (UMODULESARR.size() == 0)
				static UMODULE FREE_MOD("MAIN NOREF UMOD", "INTIME", "std module", std::string(__FILE__), std::to_string((__LINE__)));
		}
	}

	static UTAG GetNewUModuleTag()
	{
		UTAG new_tag;

		new_tag.my_mod_id = UMODULESARR.size() - 1;
		new_tag.my_mod_ptr = UMODULESARR[UMODULESARR.size() - 1];

		return new_tag;
	}






    //----------------------------------------------------------------------//
    namespace localisation
    {
        static constexpr size_t MaxGroupLen  () { return 64;        }
        static constexpr size_t MaxTagLen    () { return 64;        }
        static constexpr size_t MaxValueLen  () { return 64 * 1024; }
        static constexpr size_t ISO_639_3_Len() { return 3; }

        struct local_string;
        struct local_lang;

        struct local_string
        {
            local_string( std::string _group,
                          std::string _tag,
                          std::string _value)
            {
                group = _group;
                tag   = _tag;
                value = _value;
            }

            std::string group;		//
            std::string tag;		//
            std::string value;		//
            std::size_t ex_id;      //  uid
            std::string ex_value;   //
        };

        struct local_lang
        {
            std::string lang_name_us;    //  Lang name in eng encoding
            std::string lang_name_self;  //  Lang name in itself (utf8)
            std::string tag_iso;         //  Lang tag (ISO 639.3)
            std::size_t tag_id;          //  Id, not used yet
         };

        static const local_lang local_lang_English    = { "English",     IN_UTF8("English"),  	"eng", 0 };
        static const local_lang local_lang_Russian    = { "Russian",     IN_UTF8("Русский"),  	"rus", 0 };
        static const local_lang local_lang_Arabic     = { "Arabic",      IN_UTF8("العربية"),  	"ara", 0 };
        static const local_lang local_lang_German     = { "German",      IN_UTF8("Deutsch"),  	"deu", 0 };
        static const local_lang local_lang_French     = { "French",      IN_UTF8("Français"), 	"fra", 0 };
        static const local_lang local_lang_Portuguese = { "Portuguese",  IN_UTF8("Português"),	"por", 0 };
        static const local_lang local_lang_Spanish    = { "Spanish",     IN_UTF8("Español"),  	"spa", 0 };
        static const local_lang local_lang_Japanese   = { "Japanese",    IN_UTF8("日本語"),    	"jpn", 0 };
        static const local_lang local_lang_Chinese    = { "Chinese",     IN_UTF8("中文"),      	"zho", 0 };
        static const local_lang local_lang_Hindi      = { "Hindi",       IN_UTF8("हिन्दी"),      	"hin", 0 };

    }

}


using namespace UNIVERSAL;
using modules = std::vector<UNIVERSAL::UTAG>;

#define defhelpermix(x) _dumod_##x
#define defhelperex(x) defhelpermix(x)
#define DEF_UMODULE(Tag, Name, Ver, Desc)	namespace{static UMODULE defhelperex(Tag)(Name, Ver, Desc, std::string(__FILE__),std::to_string((__LINE__)));} //UTAG Tag = GetNewUModuleTag();
#define UM_ADDINF(...)
#define DEF_UCLASS(cName)				UNIVERSAL::UCLASS(cName, std::string(__FILE__), std::to_string(__LINE__))


 DEF_UMODULE(module_uml, "UML", "0.0.0.0",
 "Universal module loader"	"\n"
 							"\n"
 "usage:"					"\n"
 "    DEF_UMODULE(Module Name, Version, Description);");



class some_class : UCLASS
{
	some_class() : DEF_UCLASS("uclass")
	{



	}

public:
protected:
private:
};


#endif