#pragma once

#include "../shared_in.h"
#include "../graphics/gpu.drawer.h"

#ifdef GPUAPI_GL_CTX_WARPER_GLEW
	#define IMGUI_IMPL_OPENGL_LOADER_GLEW
//#elif
//#elif
//#elif
#else
	#pragma message ("Warning : cant detect any imgui impl warper")
#endif

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <3rd-party/imgui-1.70/imgui.h>
#include <3rd-party/imgui-1.70/imgui_internal.h>

#if false

	#include <3rd-party/imgui-1.70/examples/imgui_impl_dx11.h>
	#include <3rd-party/imgui-1.70/examples/imgui_impl_opengl3.h>

#elif GPU_API == __gpu_api_opengl_2

	#include <3rd-party/imgui-1.70/examples/imgui_impl_opengl2.h>

#elif GPU_API == __gpu_api_opengl_ES

	//sdl impl used now
	//#include <3rd-party/imgui-1.70/examples/imgui_impl_opengl3.h>
	#include "imimpl/imgui_impl_SDL2_GL3.h"

#endif

static bool ImGui_Platform_Init(void* window_id);
static bool ImGui_Platform_UpdateMouseCursor();

struct ImplParams;
struct ImplEvent;

class ImGuiProvider
{
public:
	ImGuiProvider()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		log::debug << "imgui provider-impl context created";
	}

	virtual ~ImGuiProvider()
	{
		ImplClose();
		ImGui::DestroyContext();
		log::debug << "imgui provider-impl context destroyed";
	}

	bool ImplInit(graphics::Drawer* drawer, ImplParams* params)
	{
		bool ok = true;

		ok &= ::ImGui_Platform_Init(nullptr);
		ok &= api_init(drawer, params);

		return ok;
	}

	void ImplClose()
	{
		api_close();
	}

	void ImplNewFrame()
	{
		ImGui::NewFrame();
		api_new_frame();
	}

	void ImplUpdateFontTex()
	{
		api_update_font_tex();
	}

	void ImplHardDraw()
	{
		api_impl_hard_draw();
	}

protected:
	bool api_init(graphics::Drawer*, ImplParams*);
	void api_close();
	void api_new_frame();
	void api_update_font_tex();
	void api_impl_hard_draw();
};


#if (GPU_API == __gpu_api_opengl_2 ) // todo mb add (|| GPU_API == __gpu_api_opengl_3)

	struct ImplParams
	{
		// not needed for GL2
		const os_window_handle os_window    = {};
 		const std::string      glsl_version = {}; // can be null
	};
	struct ImplEvent {};

	inline bool ImGuiProvider::api_init(graphics::Drawer* drawer, ImplParams* params)
	{
 		bool ok = true;

  		ok &= ::ImGui_ImplOpenGL2_Init();
 		ok &= ::ImGui_ImplOpenGL2_CreateDeviceObjects();

		return ok;
	}

	inline void ImGuiProvider::api_close           () { ::ImGui_ImplOpenGL2_Shutdown(); }
	inline void ImGuiProvider::api_new_frame       () { ::ImGui_ImplOpenGL2_NewFrame(); }
	inline void ImGuiProvider::api_update_font_tex () { ::ImGui_ImplOpenGL2_CreateFontsTexture(); }
	inline void ImGuiProvider::api_impl_hard_draw  () { ::ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData()); }

#elif (GPU_API == __gpu_api_opengl_ES)

	struct ImplParams { };
	struct ImplEvent  { SDL_Event event; };

	inline bool ImGuiProvider::api_init(graphics::Drawer* drawer, ImplParams* params)
	{
		bool ok = true;

		ok &= ::ImGui_ImplSdlGL3_Init(GetSDLGLContext().windowId);
		ok &= ::ImGui_ImplSdlGL3_CreateDeviceObjects();

		return ok;
	}

	inline void ImGuiProvider::api_close            () { ::ImGui_ImplSdlGL3_Shutdown(); }
	inline void ImGuiProvider::api_new_frame        () { ::ImGui_ImplSdlGL3_NewFrame(GetSDLGLContext().windowId); }
	inline void ImGuiProvider::api_update_font_tex  () { ::ImGui_ImplSdlGL3_CreateFontsTexture(); }
	inline void ImGuiProvider::api_impl_hard_draw   () { ::ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData()); }

#else
	#error
#endif

#if (PLATFORM_OS == OS_WINDOWS)

	static bool ImGui_Platform_Init(void* window_hwnd)
	{
		// Setup back-end capabilities flags
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		io.ImeWindowHandle = window_hwnd;

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
		io.KeyMap[ImGuiKey_Tab]			= VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow]	= VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow]	= VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow]		= VK_UP;
		io.KeyMap[ImGuiKey_DownArrow]	= VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp]		= VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown]	= VK_NEXT;
		io.KeyMap[ImGuiKey_Home]		= VK_HOME;
		io.KeyMap[ImGuiKey_End]			= VK_END;
		io.KeyMap[ImGuiKey_Insert]		= VK_INSERT;
		io.KeyMap[ImGuiKey_Delete]		= VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace]	= VK_BACK;
		io.KeyMap[ImGuiKey_Space]		= VK_SPACE;
		io.KeyMap[ImGuiKey_Enter]		= VK_RETURN;
		io.KeyMap[ImGuiKey_Escape]		= VK_ESCAPE;
		io.KeyMap[ImGuiKey_A]			= 'A';
		io.KeyMap[ImGuiKey_C]			= 'C';
		io.KeyMap[ImGuiKey_V]			= 'V';
		io.KeyMap[ImGuiKey_X]			= 'X';
		io.KeyMap[ImGuiKey_Y]			= 'Y';
		io.KeyMap[ImGuiKey_Z]			= 'Z';

		return true;
	}

	static bool ImGui_Platform_UpdateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return false;

		ImGuiMouseCursor imgui_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			::SetCursor(NULL);
		}
		else
		{
			// Hardware cursor type
			LPTSTR win32_cursor = IDC_ARROW;
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
			case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
			case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
			case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
			case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
			case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
			case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
			}

 			::SetCursor(::LoadCursor(NULL, win32_cursor));
  		}
		return true;
	}

#else

	static bool ImGui_Platform_Init(void* hwnd)    { return false; } //todo }
	static bool ImGui_Platform_UpdateMouseCursor() { return false; } //todo }

#endif