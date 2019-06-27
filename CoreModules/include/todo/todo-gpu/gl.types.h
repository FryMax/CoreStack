#pragma once

#include "../shared_in.h"

#include "gpu.types.h"

#include "../../3rd-party/glew-2.1.0/include/GL/glew.h"
#include "../../3rd-party/glwf-3.2.1/include/GLFW/glfw3.h"

#pragma comment (lib, "opengl32.lib")

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#pragma comment (lib, "opengl32.lib")

namespace graphics
{
	//////////////////////////////////////////////////////////////////////////
	struct impl_factory             {  };
	struct impl_adapter             {  };
	struct impl_output              {  };
	struct impl_device              {  }; 
	struct impl_swap_chain          {  };
								   
	struct impl_texture				{ GLuint uid_tex; };
	struct impl_buffer              {  };
	struct impl_render_target       {  };
	struct impl_depth_stencil       {  };

	struct impl_shader				{	};
 
	struct impl_shader_resourse     {   };
								   
	struct impl_sampler             {   };
	struct impl_rasterizer          {   };
	struct impl_blender				{   };
	struct impl_topology            { GLenum topology; };

	//////////////////////////////////////////////////////////////////////////

	struct gpu_factory : v_factory
	{
	public:
		virtual ~gpu_factory() {}

	protected:
		GLFWwindow* glfw_window = nullptr;

#if (GPU_API == __gpu_api_opengl_2)
		const std::string  gl_glsl_version = "#version 130";
		const unsigned int gl_ver_major = 2;
		const unsigned int gl_ver_minor = 0;
#elif (GPU_API == __gpu_api_opengl_3)
		const std::string  gl_glsl_version = "#version 320 core";
		const unsigned int gl_ver_major = 3;
		const unsigned int gl_ver_minor = 2;
#endif

#if (BUILD_MODE == __build_mode_debug)
		bool gl_debug_enabled = true;
#else
		bool gl_debug_enabled = false;
#endif
	protected:
		//--------------------------------------------------------------------------// render
		 
	};



	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class gpu_adapter : public v_adapter
	{
	private:
		impl_adapter impl;

	protected:   
		impl_adapter* api_get_adapter() { return &impl; }
		adapter_info  api_get_info   () { return adapter_info(); }
	};

	class gpu_output : public v_output
	{
	private:
		impl_output impl;

	protected:
		impl_output*             api_get_output      () { return &impl; }
		output_info	             api_get_output_info () { return {};    }
		std::vector<output_mode> api_get_output_modes() { return {};    }
	};

	class gpu_device : public v_device          
	{
	private:
		impl_device impl;

	protected:
		impl_device* api_get_device() { return &impl; }
	};

	class gpu_swap_chain : public v_swap_chain      
	{
	private:
		impl_swap_chain impl;

	protected:
		const impl_swap_chain* api_get_swap_chain() { return &impl; }
	};

	class gpu_texture : public v_texture
	{
	public:
		// todo mb dell
		const auto get_FC() { return api_get_factory();           }
		const auto get_DV() { return nullptr;                     }
		const auto get_RS() { return api_get_factory()->get_rs(); }
		
		INT2 get_size()
		{
			return INT2(w, h);
		}

	private:
		impl_texture impl;
		texture_state state; // mb better not to store this data, instead getting data straight from gpu object
	
		GLsizei w;
		GLsizei h;
		GLsizei array_size;
		GLenum	target;
		GLint	format;
		GLenum	type;
 
	protected:
		impl_texture* api_get_impl () { return &impl; }
		texture_state api_get_state() { return state; }

		void api_release()
		{
			if (impl.uid_tex != 0)
				glDeleteTextures(array_size, &impl.uid_tex);
			impl.uid_tex = 0;
		}

		bool api_bind(size_t slot) 
		{
			// slot not used
			glBindTexture(target, impl.uid_tex); 
			return true;
		}
		
		void api_unbind()
		{ 
			glBindTexture(target, 0); 
		}
 
		//todo mbdell
		bool gl_active_tex(size_t slot)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			return true;
		}

		bool api_create(texture_state opt, void* init_data)
		{
			api_release();

			w		= opt.w;
			h		= opt.h;
 			array_size = opt.array_size;
			target	= !opt.multisampled ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
  			format	= GL_RGBA;
			type	= GL_UNSIGNED_BYTE;

			const void *data = init_data;

			glGenTextures(1, &impl.uid_tex);
		
 			if (init_data)
			{		
				// mb generate mipmaps here
 				// for mimmap levels...

				GLint mipmap_level = 0;

  				glBindTexture(target, impl.uid_tex);
				{
					if (opt.bgra_mode)
					{
						GLint swizzleMask[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };
						glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
					}

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
 					glTexImage2D (target, mipmap_level, format, opt.w, opt.h, 0, format, type, data);
				}
				glBindTexture(target, 0);
			}

			log::debug << str::format::insert("{}: {} {}", __FUNCTION__, w, h);
			return true;
 		}

		bool api_update_data(void* data)
		{
 			return api_update_sub_data(QUAD(INT2(0, 0), INT2(w, h)), data);
 		}

		bool api_update_sub_data(QUAD rect, void* data)
		{
			GLint off_x		= rect.left;
			GLint off_y		= rect.top;
			GLint size_w	= rect.GetSize().x;
			GLint size_h	= rect.GetSize().y;
			GLint mipmaplvl = 0;

			glBindTexture(target, impl.uid_tex);
  			glTexSubImage2D(target, 
				mipmaplvl, 
				off_x, 
				off_y, 
				size_w, 
				size_h, 
				format, 
				type, 
				data);
 			glBindTexture(target, 0);

			return true;
		}

		bool api_copy_tex_sub_to(impl_texture* copy_target, QUAD Scr, QUAD Dst)
		{
			GLint left_bot_x = Scr.left;
			GLint left_bot_y = Scr.bottom;
			GLint size_w = Scr.GetSize().x;
			GLint size_h = Scr.GetSize().y;
		
			GLint offset_x = Dst.left - Scr.left;
			GLint offset_y = Dst.top - Scr.top;

			glBindTexture(target, impl.uid_tex);

			glCopyTexSubImage2D(copy_target->uid_tex,
				0,
				offset_x,
				offset_y,
				left_bot_x,
				left_bot_y,
				size_w,
				size_h);

			glBindTexture(target, 0);

			return true;
		}

 		bool api_resize(size_t w, size_t h)
		{
			LOG_WARN_NO_IMPL;
			return false;
		}
 
	};
	class gpu_buffer : public v_buffer 
	{
	private:
		impl_buffer impl;

	protected:
		impl_buffer* api_get_buffer() { return &impl; }
		
		void api_release() {}

		bool gpu_api_generate_buffer(GpuBufferType type, void* data, size_t arr_size, size_t obj_size, GpuAccess cpu_access)
		{
			return false;
		}
	};

	class gpu_render_target : public v_render_target   
	{
	private:
		impl_render_target impl;

	protected:
		impl_render_target* api_get_render_target() { return &impl; }

		void api_release() {}
	};

	class gpu_depth_stencil   : public v_depth_stencil   
	{
	private:
		impl_depth_stencil impl;
	
	protected:
		impl_depth_stencil* api_get_depth_stencil() { return &impl; }

		void api_release() {}
	};

	class gpu_shader : public v_shader          
	{
	private:
		impl_shader impl;
	
	protected:

		impl_shader* api_get_shader() { return &impl; }

 		bool api_compilite(GpuShaderType type, std::string asci_code, const int sem) { return false; }

		void api_release() {}

	};
	class gpu_shader_resourse : public v_shader_resourse 
	{
	private:
		impl_shader_resourse impl;

	protected:
		impl_shader_resourse* api_get_shader_resourse() { return &impl; }

		bool api_get_create_shader_resourse(impl_texture* texture) { return false; }

		void api_release() {}

	};


	class gpu_sampler : public v_sampler         
	{
	private:
		impl_sampler impl;
	 
		GLint convert_sampler_mode(TextureSamplerMode mode)
		{
			switch (mode)
			{
			case TextureSamplerMode::Near:        return GL_NEAREST;
 			case TextureSamplerMode::Linear:      return GL_LINEAR;
 			case TextureSamplerMode::Anysotropic: return GL_LINEAR; // todo
   			}
		}
		GLint convert_adress_mode(TextureAddressMode mode)
		{
			switch (mode)
			{
			case TextureAddressMode::Warp:	 return GL_REPEAT;
 			case TextureAddressMode::Mirror: return GL_MIRRORED_REPEAT;
 			case TextureAddressMode::Calm:	 return GL_CLAMP_TO_EDGE;
 			case TextureAddressMode::Border: return GL_CLAMP_TO_BORDER;
 			}
		}
	
		GLenum type;

		GLint adress_md;
 		GLint min_filter;
		GLint mag_filter;

		GLfloat bodred_color[4];

		GLfloat mip_bisas;
		GLfloat min_mip;
		GLfloat mag_mip;
 
	protected:
		impl_sampler* api_get_sampler()
		{
			return &impl;
		}
 
		virtual bool api_create(sampler_opt opt)
		{
			type = (false == opt.Multisampled) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;

			adress_md	= convert_adress_mode(opt.AdressMode);
			min_filter	= convert_sampler_mode(opt.FilterMIN);
			mag_filter	= convert_sampler_mode(opt.FilterMAG);

			mip_bisas	= opt.MipBiasOffset;
			min_mip		= opt.MipMaxLod;
			mag_mip		= opt.MipMaxLod;

			bodred_color[0] = opt.BorderColor.x / 255.0f;
			bodred_color[1] = opt.BorderColor.y / 255.0f;
			bodred_color[2] = opt.BorderColor.z / 255.0f;
			bodred_color[3] = opt.BorderColor.w / 255.0f;

			//anisotropic

			return true;
 		}

		virtual bool api_bind(size_t slot)
		{
  			glTexParameteri(type, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(type, GL_TEXTURE_MAG_FILTER, mag_filter);

			glTexParameteri(type, GL_TEXTURE_WRAP_S, adress_md);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, adress_md);
			glTexParameteri(type, GL_TEXTURE_WRAP_R, adress_md);
  
			glTexParameterf(type, GL_TEXTURE_LOD_BIAS,	mip_bisas);
			glTexParameterf(type, GL_TEXTURE_MIN_LOD,	min_mip);
			glTexParameterf(type, GL_TEXTURE_MAX_LOD,	mag_mip);

			//anisotropic
			return true;
   		}

		void api_release() {}
	};

	class gpu_rasterizer : public v_rasterizer      
	{
	private:
		impl_rasterizer impl;

	protected:
		impl_rasterizer* api_get_rasterizer()
		{
			return &impl;
		}

		void api_release() {}
	};

	class gpu_color_blender : public v_color_blender         
	{
	private:
		impl_blender impl;

	protected:
		impl_blender* api_get_blender() { return &impl; }

		void api_release() {}
	};

	class gpu_topology : public v_topology        
	{
	private:
		impl_topology impl;
	
	protected:
		impl_topology* api_get_topology() { return &impl; }

		void api_release() {}
	};

}
