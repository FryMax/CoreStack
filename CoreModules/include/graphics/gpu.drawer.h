#pragma once

#include "../shared_in.h"

#include "gpu.types.h"

#define GPUAPI_HELPER_SDL   (FORCE_SDL_USAGE ? true : false)
#define GPUAPI_TARGET_IS_GL (true)

#if GPUAPI_TARGET_IS_GL

	#if EM_BUILD
		#define GPUAPI_GL_VER_MAJOR 3
		#define GPUAPI_GL_VER_MINOR 0
	#else
		#define GPUAPI_GL_VER_MAJOR 2
		#define GPUAPI_GL_VER_MINOR 0
	#endif

	#undef GPUAPI_GL_CTX_LOADER_SDL
	#undef GPUAPI_GL_CTX_LOADER_GLWF
#else
	#error "bad gpu api target"
#endif

//---------------------------------------------------------------

#if EM_BUILD
	#include <emscripten.h>
	#include <GLES3/gl3.h>
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_syswm.h>
	#include <SDL2/SDL_opengles.h>
	#define GPUAPI_GL_CTX_LOADER_SDL
#else
	// todo mb if (gl ver <= 2.0) do not use warper
	#define GPUAPI_GL_CTX_WARPER_GLEW
	#include "../../../3rd-party/glew-2.1.0/include/GL/glew.h"

	#if (GPUAPI_HELPER_SDL == true)
		#define GPUAPI_GL_CTX_LOADER_SDL
		#include "../../../3rd-party/sdl2/include/SDL.h"
		#include "../../../3rd-party/sdl2/include/SDL_syswm.h"
		#include "../../../3rd-party/sdl2/include/SDL_opengl.h"
	#else
		#define GPUAPI_GL_CTX_LOADER_GLWF
		#include "../../../3rd-party/glwf-3.2.1/include/GLFW/glfw3.h"
	#endif

	#pragma comment (lib, "opengl32.lib")
#endif

#if (GPUAPI_HELPER_SDL == true)

namespace
{
	struct SDL_GL_Context
	{
		SDL_Window* windowId;
		SDL_GLContext sdl_gl_context;
	};

	static SDL_GL_Context& GetSDLGLContext()
	{
		static SDL_GL_Context con;
		return con;
	}
}

#endif


//-----------------------------------// cpl part

#define API_IMPL   virtual
#define API_EXTERN static

namespace
{
	struct TextureImpl { GLuint uid; };
	struct SamplerImpl { }; //todo

	API_EXTERN bool impl_Init();

	API_EXTERN bool impl_InitWarper();

	API_EXTERN void impl_ChangeVsunc(bool state);

	API_EXTERN void impl_ResizeOutput(INT2 size);

	API_EXTERN void impl_SwapBuffers();

	API_EXTERN void impl_PrintCaps();

	API_EXTERN void impl_Release();
}

namespace graphics
{
	using namespace art;
	using namespace art::color;

	namespace
	{
		struct DrawerRef : NonAssignable
		{
		public:
			void Release()
			{
				impl_release();
			}

		protected:
			API_IMPL void impl_release() {}
		};
	}

	struct Drawer : DrawerRef
	{
	public:
		static Drawer* getDrawer(Drawer* setter = nullptr)
		{
			static Drawer* getter = nullptr;

			if (setter)
			{
				DEBUG_BREAK_IF(getter);
				getter = setter;
			}

			return getter;
		}

		Drawer()
		{
			this->getDrawer(this);
		}

		virtual ~Drawer() {}

	protected:
		API_IMPL bool impl_init           (RenderOptions);
		API_IMPL void impl_change_vsunc   (bool state);
		API_IMPL void impl_change_fullscr (bool state);
		API_IMPL void impl_output_resize  (const INT2& size);
		API_IMPL void impl_output_clean   (const color4f& color);
		API_IMPL void impl_output_present ();
		API_IMPL void impl_print_caps     (); //todo make usual caps return
		API_IMPL void impl_release        ();
	};

	struct GpuSampler : DrawerRef
	{
	protected:
		SamplerState state;

	public:
		bool CreateSampler(const SamplerState& _state)
		{
			if (impl_check_state(_state))
				return false;
			else
				state = _state;

			impl_create_sampler();

			return true;
		}

	protected:
		API_IMPL bool impl_check_state    (const SamplerState& state);
		API_IMPL void impl_create_sampler ();
		API_IMPL void impl_release        ();
	};

	struct GpuTexture : DrawerRef
	{
	protected:
		TextureState state = TextureState();
		TextureImpl  impl;

	public:
		virtual ~GpuTexture() { Release(); }

		//--------- get

		const TextureImpl* GetImpl() const { return &impl; }

		INT2 GetSize() const { return INT2{ state.w, state.h }; }

		bool GetData(bitmap32& out, bool auto_resize_output = true)
		{
			if (out.size() != INT2{ state.w, state.h })
			{
				if (auto_resize_output)
					out.resize(INT2{ state.w, state.h }, scalefilter::no_scale);
				else
					return false; // output inst cant fit image
			}

			impl_get_data(reinterpret_cast<void*>(out.raw_data()));

			return true;
		}

		//--------- set

		bool Update(const bitmap32& bitmap, const TextureState* _state = nullptr)
		{
			const void* init_data = bitmap.raw_data();

			if (GetSize() != bitmap.size())
			{
				// different size not expected
				// todo mb call art::scale
				if (_state)
				{
					DEBUG_BREAK_IF(bitmap.size().x == _state->w
						        && bitmap.size().y == _state->h);

					if (!impl_check_state(*_state))
					{
						return false;
					}
					else
					{
						state = *_state;
					}
				}

				log::trace << str::format::insert("Update (resize) {}-{} to {}-{}",
					state.w, state.h, bitmap.size().x, bitmap.size().y);

				state.w = bitmap.size().x;
				state.h = bitmap.size().y;

				impl_release();
				impl_create(init_data);
			}
			else
			{
				impl_update_data(init_data);
			}

			return true;
		}

		void SetSampler(const SamplerState& state)
		{
			impl_set_sampler(state);
		}

	protected:
		API_IMPL bool impl_check_state (const TextureState& state);
		API_IMPL void impl_create      (const void* pData = nullptr);
		API_IMPL void impl_update_data (const void* pData = nullptr);
		API_IMPL void impl_bind        (unsigned slot = 0);
		API_IMPL void impl_unbind      ();
		API_IMPL void impl_set_sampler (const SamplerState& sampler);
		API_IMPL void impl_release     ();

		//-- not sure
		API_IMPL void impl_get_data(void* pOut);
	};
}

//-----------------------------------// virt part end
//-----------------------------------// impl part begin

#if (GPU_API == __gpu_api_opengl_2) || (GPU_API == __gpu_api_opengl_3) || (GPU_API == __gpu_api_opengl_ES)

	namespace graphics
	{
		//////////////////////////////////////////////////////////////////////////
		//  Drawer
		//////////////////////////////////////////////////////////////////////////

		inline bool Drawer::impl_init(RenderOptions RO)
		{
			bool ret = impl_Init();
			impl_print_caps();
			return ret;
		}

		inline void Drawer::impl_change_vsunc(bool state)
		{
			impl_ChangeVsunc(state);
		}

		inline void Drawer::impl_change_fullscr(bool state)
		{
			LOG_WARN_NO_IMPL;
		}

		inline void Drawer::impl_output_resize(const INT2& size)
		{
			glViewport(0, 0, size.x, size.y);
		}

		inline void Drawer::impl_output_clean(const color4f& color)
		{
			unsigned int flags = 0;

			flags |= GL_COLOR_BUFFER_BIT;
			//flags |= GL_DEPTH_BUFFER_BIT;
			//flags |= GL_STENCIL_BUFFER_BIT;

			glClearColor(color.x, color.y, color.z, color.w);
			glClear(flags);
		}

		inline void Drawer::impl_output_present()
		{
			impl_SwapBuffers();
		}

		inline void Drawer::impl_print_caps()
		{
			int max_ts;
			int max_img_uints;

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_ts);
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_img_uints);

			std::string caps_str = str::format::insert(
				"OpenGL device caps:               \n"
				"  GL_MAX_TEXTURE_SIZE:        | {}\n"
				"  GL_MAX_TEXTURE_IMAGE_UNITS  | {}\n"
				"  GL_VERSION                  | {}\n"
				"  GL_SHADING_LANGUAGE_VERSION | {}",
					max_ts,
					max_img_uints,
					std::string((const char*)glGetString(GL_VERSION)),
					std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

			log::debug << caps_str;

			impl_PrintCaps();
		}

		inline void Drawer::impl_release()
		{
			impl_Release();
		}

		//////////////////////////////////////////////////////////////////////////
		//  GpuSampler
		//////////////////////////////////////////////////////////////////////////

		inline bool GpuSampler::impl_check_state(const SamplerState& state)
		{
			LOG_WARN_NO_IMPL;
			return false;
		}

		inline void GpuSampler::impl_create_sampler()
		{
			LOG_WARN_NO_IMPL;
		}

		inline void GpuSampler::impl_release()
		{
			LOG_WARN_NO_IMPL;
		}

		//////////////////////////////////////////////////////////////////////////
		//  GpuTexture
		//////////////////////////////////////////////////////////////////////////

		inline bool GpuTexture::impl_check_state(const TextureState& state)
		{
			assert(state.w > 0);
			assert(state.h > 0);
			assert(state.multisampled == false);
			//...
			return true;
		}

		inline void GpuTexture::impl_create(const void* pData)
		{
			const GLsizei w      = this->state.w;
			const GLsizei h      = this->state.h;
			const GLsizei n      = this->state.array_size;
			const GLenum  target = GL_TEXTURE_2D;
			const GLint   format = GL_RGBA;
			const GLenum  type   = GL_UNSIGNED_BYTE;

			glGenTextures(n, &this->impl.uid);
			log::trace << str::format::insert("glGenTextures: {} glGetError:{}", this->impl.uid, glGetError());
			DEBUG_BREAK_IF(this->impl.uid == 0);


			if (pData)
			{
				// mb generate mipmaps here
				// for each mimmap level
				GLint mipmap_level_target = 0;

				glBindTexture(target, this->impl.uid);
				{
					if (this->state.bgra_mode)
					{
						GLint swizzleMask[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };
						glTexParameteriv(GL_TEXTURE_2D, 0x8E46/*GL_TEXTURE_SWIZZLE_RGBA*/, swizzleMask);
					}

					// TODO not supported by (em/gl2/webgl1)
					// glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					// TODO used GL_CLAMP_TO_EDGE case (em/gl2/webgl1) not supports NPOT tex with 'warp' warpers
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

					glTexImage2D(target, mipmap_level_target, format, w, h, 0, format, type, pData);
				}
				glBindTexture(target, 0);
			}
		}

		inline void GpuTexture::impl_update_data(const void* pData)
		{
			GLint pos_x  = 0;
			GLint pos_y  = 0;
			GLint len_w = this->state.w;
			GLint len_h = this->state.h;
			GLint mip_target = 0;

			glBindTexture(GL_TEXTURE_2D, this->impl.uid);
			{
				glTexSubImage2D(GL_TEXTURE_2D,
					mip_target,
					pos_x,
					pos_y,
					len_w,
					len_h,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					pData);
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		inline void GpuTexture::impl_bind(unsigned slot)
		{
			// todo slot arg not used
			glBindTexture(GL_TEXTURE_2D, this->impl.uid);
		}

		inline void GpuTexture::impl_unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		inline void GpuTexture::impl_set_sampler(const SamplerState& sampler)
		{
			static auto convert_sampler_mode = [](TextureSamplerMode mode) -> GLint
			{
				switch (mode)
				{
				case TextureSamplerMode::Near:        return GL_NEAREST;
				case TextureSamplerMode::Linear:      return GL_LINEAR;
				case TextureSamplerMode::Anysotropic: return GL_LINEAR; // TODO, anysotropic not supported in gl-2
				}
				assert(false);
				return GL_NEAREST;
			};

			static auto convert_adress_mode = [](TextureAddressMode mode) -> GLint
			{
				switch (mode)
				{
				case TextureAddressMode::Warp:	 return GL_REPEAT;
				case TextureAddressMode::Mirror: return GL_MIRRORED_REPEAT;
				case TextureAddressMode::Calm:	 return GL_CLAMP_TO_EDGE;
				case TextureAddressMode::Border: return 0x812D;//GL_CLAMP_TO_BORDER; TODO not supported in gl-es
				}
				assert(false);
				return GL_REPEAT;
			};

			glBindTexture(GL_TEXTURE_2D, this->impl.uid);
			{
				GLint min_filter = convert_sampler_mode(sampler.FilterMIN);
				GLint mag_filter = convert_sampler_mode(sampler.FilterMAG);
				GLint adress_md  = convert_adress_mode(sampler.AdressMode);
				GLfloat mip_off  = sampler.MipBiasOffSet;
				GLfloat mip_min = sampler.MipMinLod;
				GLfloat mip_mag = sampler.MipMaxLod;

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, adress_md);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, adress_md);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, adress_md);

				glTexParameterf(GL_TEXTURE_2D, 0x8501/*TODO GL_TEXTURE_LOD_BIAS*/, mip_off);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD,  mip_min);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD,  mip_mag);

				//set anisotropic params, TODO anisotropic not supported on gl-2
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		inline void GpuTexture::impl_release()
		{
			if (this->impl.uid != 0)
				glDeleteTextures(this->state.array_size, &this->impl.uid);

			this->impl.uid = 0;
		}

		inline void GpuTexture::impl_get_data(void* pOut)
		{
			glBindTexture(GL_TEXTURE_2D, this->impl.uid);

			#if (EM_BUILD || FAKE_EM_BUILD)
				// TODO
				// try https://stackoverflow.com/questions/53993820/opengl-es-2-0-android-c-glgetteximage-alternative
				// glReadPixels...
				log::warn << "glGetTexImage not supported on gl-es";
			#else
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pOut);
			#endif
		}
	}
#else
	#error "bad gpu ref impl"
#endif


//-----------------------------------// LOADER
#if defined(GPUAPI_GL_CTX_LOADER_GLWF)

	namespace
	{
		static GLFWwindow* GLWF_WINDOW = nullptr;
		static constexpr bool GL_DEBUG_MODE = (BUILD_MODE == BUILD_MODE_DEBUG);
		static const char WND_CLASS_TITLE[] = "ws-window-class@Window";

		API_EXTERN void glfw_ErrorCallback(int error, const char* description)
		{
			log::error << str::format::insert("OpenGL: glfw error {}: {}", error, description);
		}

		API_EXTERN bool impl_Init()
		{
			glfwSetErrorCallback(glfw_ErrorCallback);

			if (!new_glfwInit())
				return false;

			if (GL_DEBUG_MODE)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GPUAPI_GL_VER_MAJOR);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GPUAPI_GL_VER_MINOR);

			if (false)
			{
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			}

			//  non-standard
			GLWF_WINDOW = new_glfwCreateWindow(800, 600, WND_CLASS_TITLE, NULL, NULL);

			if (GLWF_WINDOW)
				log::debug << str::format::insert("OpenGL: context created ({}.{}) (loader:glfw)", GPUAPI_GL_VER_MAJOR, GPUAPI_GL_VER_MINOR);
			else
			{
				log::error << str::format::insert("OpenGL: connecting to window failed");

				glfwTerminate();
				return false;
			}

			if (GL_DEBUG_MODE)
			{
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

				if (GPUAPI_GL_VER_MAJOR >= 4 && GPUAPI_GL_VER_MINOR)
				{
					GLDEBUGPROC debuf_fn = nullptr;
					glDebugMessageCallback(debuf_fn, nullptr);
					glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
				}

			}

			glfwMakeContextCurrent(GLWF_WINDOW);
			//=================================// glew init part
			impl_InitWarper();

			return true;
		}

		API_EXTERN void impl_ChangeVsunc(bool state)
		{
			glfwSwapInterval(state ? TRUE : FALSE);
		}

		API_EXTERN void impl_ResizeOutput(INT2 size)
		{
			DEBUG_BREAK();
		}

		API_EXTERN void impl_SwapBuffers()
		{
			glfwMakeContextCurrent(GLWF_WINDOW);
			glfwSwapBuffers(GLWF_WINDOW);
		}

		API_EXTERN void impl_PrintCaps()
		{
			//todo
		}

		API_EXTERN void impl_Release()
		{
			glfwTerminate();
		}
	}

#elif defined(GPUAPI_GL_CTX_LOADER_SDL)

	namespace
	{
		API_EXTERN bool impl_Init()
		{
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GPUAPI_GL_VER_MAJOR);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GPUAPI_GL_VER_MINOR);

			GetSDLGLContext().sdl_gl_context = SDL_GL_CreateContext(GetSDLGLContext().windowId);

			return (impl_InitWarper()
				&& GetSDLGLContext().sdl_gl_context);
		}

		API_EXTERN void impl_ChangeVsunc(bool state)
		{
			SDL_GL_SetSwapInterval(state ? GL_TRUE : GL_FALSE);
		}

		API_EXTERN void impl_ResizeOutput(INT2 size)
		{
			DEBUG_BREAK();
		}

		API_EXTERN void impl_SwapBuffers()
		{
			SDL_GL_SwapWindow(GetSDLGLContext().windowId);
		}

		API_EXTERN void impl_PrintCaps()
		{
			//todo
		}

		API_EXTERN void impl_Release()
		{
			SDL_GL_DeleteContext(GetSDLGLContext().sdl_gl_context);
		}
	}

#else
	#error "bad gl api loader"
#endif
//-----------------------------------// WARPER
#if defined(GPUAPI_GL_CTX_WARPER_GLEW)
	namespace
	{
		API_EXTERN bool impl_InitWarper()
		{
			//glewExperimental = GL_TRUE;
			if (glewInit() == GLEW_OK)
				log::debug << str::format::insert("OpenGL: extension warper inited (loader: glew)");
			else
			{
				log::error << str::format::insert("OpenGL: failed to init extension warper (loader: glew)");
				return false;
			}
			return true;
		}
	}
#else
	namespace
	{
		API_EXTERN bool impl_InitWarper()
		{
			log::warn << str::format::insert("OpenGL: extension warper is missing");
			return true;
		}
	}
#endif




