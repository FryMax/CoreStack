#pragma once
 
#include "../shared_in.h"

#include "gpu.api.h"

namespace gpu_api
{
	inline void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::stringstream ss;

		ss << "gl debug: (" << id << "): " << message << std::endl;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             ss << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ss << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: ss << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     ss << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     ss << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           ss << "Source: Other"; break;
		} 
		ss << std::endl;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               ss << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ss << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ss << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         ss << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         ss << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              ss << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          ss << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           ss << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               ss << "Type: Other"; break;
		} 
		ss << std::endl;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         ss << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       ss << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          ss << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: ss << "Severity: notification"; break;
		} 
		ss << std::endl;
	
		log::debug << ss.str();
	}

	struct gpu_factory_opengl : v_factory
	{
	public:
		virtual ~gpu_factory_opengl(){}
		
	protected:
		GLFWwindow* glfw_window = nullptr;
	
#if (GPU_API == __gpu_api_opengl_2)
		const std::string  gl_glsl_version = "#version 130";
		const unsigned int gl_ver_major = 2;
		const unsigned int gl_ver_minor = 0;
#endif
#if (GPU_API == __gpu_api_opengl_3)
		const std::string  gl_glsl_version = "#version 320 core";
		const unsigned int gl_ver_major = 3;
		const unsigned int gl_ver_minor = 2;
#endif

#if (BUILD_MODE == __build_mode_debug)
		bool gl_debug_enabled = true;
#else
		bool gl_debug_enabled = false;
#endif
	
		static void glfw_error_callback(int error, const char* description)
		{
			log::error << str::format::insert("OpenGL: glfw error {}: {}", error, description);
		}

	public:

		virtual bool gpu_api_check_msaa_settings(size_t samples, size_t qality, std::string& err_report)
		{
			return true;
		}

	 protected:

		 IMPL_CALL virtual bool gpu_api_change_v_sunc(bool enabled)
		 {
			 if (enabled)
				 glfwSwapInterval(enabled ? GL_TRUE : GL_FALSE);

			 return true;
		 }	 

		IMPL_CALL std::vector<gpu_adapter> enum_gpu_adapters()				{ return {}; };
		IMPL_CALL std::vector<gpu_output>  enum_adapter_outputs(gpu_adapter*) { return {}; };

		//--------------------------------------------------------------------------// render

		IMPL_CALL bool gpu_api_present_render() { return true;  };

		IMPL_CALL bool gpu_api_resize_render(size_t vw, size_t vh, size_t w, size_t h) { return true; };

		IMPL_CALL bool gpu_api_change_fullscreen_state(bool state) { return true; };

		IMPL_CALL bool gpu_api_get_fullscreen_state() { return true; };

		//--------------------------------------------------------------------------// render targets

		IMPL_CALL void gpu_api_bind_render_targets(impl_render_target*, impl_depth_stencil*) 
		{
	
	
		};

		IMPL_CALL void gpu_api_clear_render_target(FLOAT4 color, impl_render_target*) 
		{
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_COLOR_BUFFER_BIT);
		};

		IMPL_CALL void gpu_api_clear_depth_stencil(DepthStencilClearMode mode, impl_depth_stencil*)
		{
 			GLbitfield clear_mode = 0;
			switch (mode)
			{
			case DepthStencilClearMode::ClearDepth:	  clear_mode = GL_DEPTH_BUFFER_BIT;							break;
			case DepthStencilClearMode::ClearStencil: clear_mode = GL_STENCIL_BUFFER_BIT;						break;
			case DepthStencilClearMode::ClearFull:    clear_mode = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;	break;
			}

			glClear(clear_mode);
		};

		//--------------------------------------------------------------------------// draw calls

		IMPL_CALL void gpu_api_draw(size_t point_count) {};
		IMPL_CALL void gpu_api_draw_indexed(size_t index_count) {};
		//virtual void gpu_api_draw_indexed_instansed(size_t index_count) = 0;

		  //--------------------------------------------------------------------------// init

		IMPL_CALL bool gpu_api_init_device      () 
		{
 			glfwSetErrorCallback(glfw_error_callback);
		
   			if (!new_glfwInit())
				return false;
 		 
			if (gl_debug_enabled)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_ver_major);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_ver_minor);
			//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 			//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

			log::debug << str::format::insert("OpenGL: context created ({}.{})", gl_ver_major, gl_ver_minor);
   
			GLFWwindow* window = new_glfwCreateWindow(1440, 900, "ws-window-class@Window", NULL, NULL);
			if (window == NULL)
			{
				log::error << str::format::insert("OpenGL: connecting to window failed");

 				glfwTerminate();
				return false;
			}
		
			//GLint flags; 
			//glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
			//if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			if (gl_debug_enabled)
			{
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			
				if (gl_ver_major >= 4 && gl_ver_minor)
				{
					glDebugMessageCallback(glDebugOutput, nullptr);
					glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
				}
			
			}
		
			glfwMakeContextCurrent(window);

 			glfw_window = window;

 			if (mRS.VSunc == true)
				glfwSwapInterval(TRUE);  
			else
				glfwSwapInterval(FALSE);  
  		 
			//=================================// glew inut part
			//glewExperimental = GL_TRUE;
			if (glewInit() != GLEW_OK)
			{
				log::error << str::format::insert("OpenGL: failed to init extension warper");
 				return false;
			}
		
			return true;
		};    //	create Device end
	
	};
}