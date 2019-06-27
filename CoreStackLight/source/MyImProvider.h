#pragma once

#include "in.h"

#if EM_BUILD
	#define GL_VER_MAJOR 2
	#define GL_VER_MINOR 0
#else
	#define GL_VER_MAJOR 2
	#define GL_VER_MINOR 0
#endif

struct MyImGuiProvider : ImGuiProvider
{
protected:
	bool api_init(gpu_api::used_gpu_factory* factory, ImplParams* params)
	{

	}
	void api_close() {}
	void api_new_frame();
	void api_update_font_tex();
	void api_impl_hard_draw();
};

 