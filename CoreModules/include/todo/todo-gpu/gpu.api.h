#pragma once

#include "../shared_in.h"

#if (GPU_API == __gpu_api_dx_11)
	#include "3d.types.dx11.h"
#endif 

#if (GPU_API == __gpu_api_opengl_2) || (GPU_API == __gpu_api_opengl_3)
	#include "gl.types.h"
#endif 

#if (GPU_API == __gpu_api_software)
  
	class gpu_adapter{};
	class gpu_output{};
	class gpu_device{};
	class gpu_swap_chain{};

	class gpu_texture{};
	class gpu_buffer{};
	class gpu_render_target{};
	class gpu_depth_stencil{};

	class gpu_shader{};
	class gpu_shader_resourse{};


	class gpu_sampler{};
	class gpu_rasterizer{};
	class gpu_color_blender{};
	class gpu_topology {};

#endif

#define IMPL_CALL

//////////////////////////////////////////////////////////////////////////
enum class srv_type
{
    undefined,
    texture,
    constant_buffer,
};

namespace graphics
{
 
}