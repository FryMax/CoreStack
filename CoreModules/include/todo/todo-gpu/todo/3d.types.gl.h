#pragma once

#include "../shared_in.h"

#include "gpu_api.types.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gpu_api
{
	//////////////////////////////////////////////////////////////////////////

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
	struct impl_blender		{   };
	struct impl_topology            { GLenum topology; };
	
	//////////////////////////////////////////////////////////////////////////
	class gpu_adapter : public v_adapter
	{
	private:
		impl_adapter impl;
	protected:   

		impl_adapter* api_get_adapter()
		{
			return &impl;
		}
		adapter_info  api_get_info()
		{
			adapter_info out;

	//         DXGI_ADAPTER_DESC Desc;
	//         impl.pAdapter->GetDesc(&Desc);
	// 
	//         std::string gpu_card_name   = str::convert::wstring_to_string(Desc.Description);
	//         std::string gpu_card_luid   = std::to_string(Desc.AdapterLuid.HighPart) + "x" + std::to_string(Desc.AdapterLuid.LowPart);
	//         std::size_t GpuMemOwn       = (Desc.DedicatedVideoMemory  );//  / 1024 / 1024);	// ����������� ������ ���������� ��� ������� ��
	//         std::size_t GpuMemLended    = (Desc.DedicatedSystemMemory );//  / 1024 / 1024);	// �������������� ����������� ��������� ������ ��� ������� ��
	//         std::size_t GpuMemShared    = (Desc.SharedSystemMemory    );//  / 1024 / 1024);	// ����� ��������� ������ ����������� � ����������� � �������� ��
	// 
	//         out.description         = gpu_card_name;
	//         out.vendor_id           = Desc.VendorId;
	//         out.device_id           = Desc.DeviceId;
	//         out.subsys_id           = Desc.SubSysId;
	//         out.revision            = Desc.Revision;
	//         out.dedicated_video_mem = GpuMemOwn;
	//         out.dedicated_sys_mem   = GpuMemLended;
	//         out.shared_system_mem   = GpuMemShared;
	//         out.adapter_luid        = gpu_card_luid;

			return out;
		}
 

		void api_release() 
		{
			//_RELEASE(impl.pAdapter);
		}
	};

	class gpu_output : public v_output
	{
	private:
		impl_output impl;
	protected:
		impl_output*              api_get_output()
		{
			return &impl;
		}
		output_info               api_get_output_info()
		{
			output_info out;

			//DXGI_OUTPUT_DESC Desc;
			//impl.pOutput->GetDesc(&Desc);
			//
			//float rotation = 0;
			//switch (Desc.Rotation)
			//{
			//case DXGI_MODE_ROTATION_UNSPECIFIED: rotation = -1;     break;
			//case DXGI_MODE_ROTATION_IDENTITY:    rotation = 0;      break;
			//case DXGI_MODE_ROTATION_ROTATE90:    rotation = 90;     break;
			//case DXGI_MODE_ROTATION_ROTATE180:   rotation = 180;    break;
			//case DXGI_MODE_ROTATION_ROTATE270:   rotation = 270;    break;
			//default:                             rotation = 0;      break;
			//}
			//
			//out.description          = str::convert::WcharToChar(Desc.DeviceName);
			//out.os_AttachedToDesktop = Desc.AttachedToDesktop;
			//out.os_Rotation          = rotation;
			//out.os_hMonitor          = new HMONITOR(Desc.Monitor);
			//out.os_Coord             = QUAD{
			//    Desc.DesktopCoordinates.top,
			//    Desc.DesktopCoordinates.left,
			//    Desc.DesktopCoordinates.bottom,
			//    Desc.DesktopCoordinates.right
			//};
        
			return out;
		}
		std::vector<output_mode>  api_get_output_modes() 
		{
			std::vector<output_mode> out;

			//RESULT OK;
			//UINT outputModesCount = 0;
			//DXGI_MODE_DESC* displayModeList = nullptr;
			//
			//OK << impl.pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &outputModesCount, NULL);
			//displayModeList = new DXGI_MODE_DESC[outputModesCount];
			//OK << impl.pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &outputModesCount, displayModeList);
			//
			//out.resize(outputModesCount);
			//ForArray(i, outputModesCount)
			//{
			//    out[i] = output_mode{
			//		displayModeList[i].Width,              
			//		displayModeList[i].Height,
			//		displayModeList[i].RefreshRate.Numerator,
			//        displayModeList[i].RefreshRate.Denominator
			//		//...
			//     };
			//}
			//
			//_DELETE_ARRAY(displayModeList);

			return out;
		}

		void api_release() 
		{
			//_RELEASE(impl.pOutput);
		}
	};
	class gpu_device : public v_device          
	{
	private:
		impl_device impl;
	protected:
		impl_device* api_get_device()
		{
			return &impl;
		}

		void api_release()
		{
			// _RELEASE(impl.pDevice);
			// _RELEASE(impl.pContext);
		}
	};
	class gpu_swap_chain : public v_swap_chain      
	{
	private:
		impl_swap_chain impl;
	protected:
		const impl_swap_chain* api_get_swap_chain()
		{
			return &impl;
		}

		void api_release()
		{
			// _RELEASE(impl.pSwapchain);
		}
	};

	class gpu_texture : public v_texture
	{
	public:

		auto getf() { return ref_api_get_my_factory();	}
		auto getd() { return ref_api_get_my_device();	}
		auto getr() { return ref_api_get_my_rs();		}





	private:
		impl_texture impl;
		texture_state state; // mb better not to store this data
	
		GLsizei w;
		GLsizei h;
		GLsizei array_size;
		GLenum	target;
		GLint	format;
		GLenum	type;
 
	protected:

		INT2 get_size() 
		{
			return INT2(w, h);
		}

		void _bind()   { glBindTexture(target, impl.uid_tex); }
		void _unbind() { glBindTexture(target, 0); }

	protected:

		void api_release() 
		{
 			if (impl.uid_tex != 0)
				glDeleteTextures(1, &impl.uid_tex);
			impl.uid_tex = 0;
		}
 
		impl_texture* api_get_impl()
		{
			return &impl;
		}

		texture_state api_get_state()
		{
			return state;
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
		
 			if (TRUE || init_data)
			{		
				// mb generate mipmaps here
 				// for mimmap levels...

				GLint mipmap_level = 0;

  				glBindTexture(target, impl.uid_tex);
			
				if (opt.bgra_mode)
				{
					GLint swizzleMask[] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };
					glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
				}

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
 				glTexImage2D (target, mipmap_level, format, opt.w, opt.h, 0, format, type, data);
			
				glBindTexture(target, 0);

				//glBindTexture(target, 0);
			}
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

			log::debug << str::format::insert("{}: {} {}", __FUNCTION__, w, h);
			return true;
 		}

		void gl_activate(size_t slot)
		{
  			glActiveTexture(GL_TEXTURE0 + slot);
 		}

		bool api_update_data(void* data)
		{
 			return api_update_sub_data(QUAD(INT2(0, 0), INT2(w, h)), data);
 		}

		bool api_update_sub_data(QUAD rect, void* data)
		{
			GLint off_x		= rect.left;
			GLint off_y		= rect.top;
			GLint size_w	= rect.get_size().x;
			GLint size_h	= rect.get_size().y;
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
			GLint size_w = Scr.get_size().x;
			GLint size_h = Scr.get_size().y;
		
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

		bool gpu_api_generate_buffer(GpuBufferType type, void* data, size_t arr_size, size_t obj_size, gpuAccess cpu_access)
		{
			//D3D11_BUFFER_DESC	   desc = {};
			//D3D11_SUBRESOURCE_DATA Data = {};
 			//
			//if (type == gpu_buffer_type::undefined)			__debugbreak();	//TODO
 			//if (type == gpu_buffer_type::vertex_buffer)		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			//if (type == gpu_buffer_type::index_buffer)		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			//if (type == gpu_buffer_type::constant_buffer)   desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			////if (type == ...)
			////////////////////////////////////////////////////////////////////////////
			//if (cpu_access == Access::no_access)	desc.Usage = D3D11_USAGE_DEFAULT;
			//if (cpu_access == Access::write)		desc.Usage = D3D11_USAGE_DYNAMIC;
			//if (cpu_access == Access::read)			desc.Usage = D3D11_USAGE_STAGING;
			//if (cpu_access == Access::read_write)	desc.Usage = D3D11_USAGE_STAGING;
			////////////////////////////////////////////////////////////////////////////
			//if (cpu_access == Access::no_access)	desc.CPUAccessFlags = 0;
			//if (cpu_access == Access::write)		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			//if (cpu_access == Access::read)			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			//if (cpu_access == Access::read_write)	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			//
			//desc.ByteWidth			 = arr_size * obj_size;
			//desc.MiscFlags			 = 0;			//D3D11_RESOURCE_MISC_FLAG 
			//desc.StructureByteStride = obj_size;
   			//
			//Data.pSysMem			= data;
			//Data.SysMemPitch		= 0;
			//Data.SysMemSlicePitch	= 0;
 			//
 			//_RELEASE(impl.pBuffer);
			//
			//RESULT OK;
			//OK << ref_api_get_my_device()->pDevice->CreateBuffer(&desc, &Data, &impl.pBuffer);
			//
			//return OK.IsSucceed();
		}




		impl_buffer* api_get_buffer() 
		{
			return &impl;
		}

		void api_release()
		{
			//_RELEASE(impl.pBuffer);
		}
	};
	class gpu_render_target   : public v_render_target   
	{
	private:
		impl_render_target impl;
	protected:
		impl_render_target* api_get_render_target()
		{
			return &impl;
		}

		void api_release()
		{
			//_RELEASE(impl.pTarget);
		}
	};
	class gpu_depth_stencil   : public v_depth_stencil   
	{
	private:
		impl_depth_stencil impl;
	protected:
		impl_depth_stencil* api_get_depth_stencil()
		{
			return &impl;
		}

		void api_release()
		{
			//_RELEASE(impl.pDepth);
		}
	};

	class gpu_shader : public v_shader          
	{
	private:
		impl_shader impl;
	protected:

		impl_shader* api_get_shader() override
		{
			return &impl;
		}

 		bool api_compilite(GpuShaderType type, std::string asci_code, vert::vertex::Semantic sem) override
		{
	//		const std::string dx_11_shader_version = "5_0";
	//		const UINT        dx_11_shader_flags	 = D3DCOMPILE_ENABLE_STRICTNESS
	//#ifdef _DEBUG
	//			| D3DCOMPILE_DEBUG
	//			| D3DCOMPILE_WARNINGS_ARE_ERRORS;
	//#else
	//			| D3DCOMPILE_PREFER_FLOW_CONTROL
	//			| D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR
	//			| D3DCOMPILE_OPTIMIZATION_LEVEL3
	//			| D3DCOMPILE_PARTIAL_PRECISION;
	//#endif
	//		//////////////////////////////////////////////////////////////////////////
	//		std::string func_name = "main";
	//		if (type == gpu_shader_type::vertex_shader) func_name = "VS";
	//		if (type == gpu_shader_type::pixel_shader)  func_name = "PS";
	//		//...
	//
	//  		std::string shader_version = "";
	//		if (type == gpu_shader_type::vertex_shader) shader_version = "vs_" + dx_11_shader_version;
	//		if (type == gpu_shader_type::pixel_shader)  shader_version = "ps_" + dx_11_shader_version;
	//		//...
	//		
	//		UINT compilite_flags = dx_11_shader_flags;
	//		
	//
	// 		//////////////////////////////////////////////////////////////////////////
	//		ID3DBlob* code	 = nullptr;
	//		ID3DBlob* errors = nullptr;
	//
	//		RESULT OK;
	// 		OK << D3DCompile(asci_code.c_str(), asci_code.size(), NULL, NULL, NULL, func_name.c_str(), shader_version.c_str(), compilite_flags, 0, &code, &errors);
	//
	//		if (OK.IsFailed())
	//		{
	//			log::error << (string)(char*)errors->GetBufferPointer();
	//			_RELEASE(errors);			
	//			_RELEASE(code);
	// 			return false;
	//		}
	// 		_RELEASE(errors);
	//		//------------------------------------------------------------------// create shader  
	//		switch (type)
	//		{
	//		case gpu_shader_type::undefined:		{ OK << false; __debugbreak(); }																							  	  break;
	// 		case gpu_shader_type::vertex_shader:	{ OK << ref_api_get_my_device()->pDevice->CreateVertexShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &impl.pVshader); 	} break;
	// 		case gpu_shader_type::pixel_shader:		{ OK << ref_api_get_my_device()->pDevice->CreatePixelShader (code->GetBufferPointer(), code->GetBufferSize(), NULL, &impl.pPshader); 	} break;
	// 		}																																									   
	//
	//		if (OK.IsFailed())
	//		{
	//			_RELEASE(code);
	//			_RELEASE(impl.pVshader);
	//			_RELEASE(impl.pPshader);
	//			return false;
	//		} 
	// 		//------------------------------------------------------------------// CREATE IL (only for vs shaders)
	//		if (type == gpu_shader_type::vertex_shader)
	//		{
	// 			u3d::vertex::Semantic semantic = sem;
	//			std::vector<D3D11_INPUT_ELEMENT_DESC> input_desc;
	//
	//			ForArray(i, semantic.size())
	//			{
	//				const char* tag		= semantic[i].tag;
	//				DXGI_FORMAT format	= DXGI_FORMAT_UNKNOWN;
	//
	//				switch (semantic[i].type)
	//				{
	//				case u3d::vertex::vo_type::FLOAT_1:    format = DXGI_FORMAT_R32_FLOAT;			break;
	//				case u3d::vertex::vo_type::FLOAT_2:    format = DXGI_FORMAT_R32G32_FLOAT;		break;
	//				case u3d::vertex::vo_type::FLOAT_3:    format = DXGI_FORMAT_R32G32B32_FLOAT;	break;
	//				case u3d::vertex::vo_type::FLOAT_4:    format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	//				}
	//				D3D11_INPUT_ELEMENT_DESC ied = { tag, 0, format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	//				input_desc.push_back(ied);
	// 			}
	//
	//			ID3D11InputLayout* layout = nullptr;
	//			OK << ref_api_get_my_device()->pDevice->CreateInputLayout(&input_desc[0], input_desc.size(), code->GetBufferPointer(), code->GetBufferSize(), &impl.pLayout);
	//			if (OK.IsFailed())
	//			{
	//				_RELEASE(code);
	//				_RELEASE(impl.pVshader);
	//				_RELEASE(impl.pPshader);
	//				_RELEASE(impl.pLayout);
	//				return false;
	//			}
	//
	// 		}
	//		//------------------------------------------------------------------// CLEAR AND RETURN
	//		_RELEASE(code);

 
			return true;
		}

		void api_release()
		{
			//	_RELEASE(impl.pVshader);
			//	_RELEASE(impl.pPshader);
		}

	};
	class gpu_shader_resourse : public v_shader_resourse 
	{
	private:
		impl_shader_resourse impl;
	protected:
		impl_shader_resourse* api_get_shader_resourse()
		{
			return &impl;
		}

		bool api_get_create_shader_resourse(impl_texture* texture) 
		{
			//D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			//
			//desc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
			//desc.ViewDimension				= (ref_api_get_my_rs()->MSAA_SamplerCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
 			//desc.Texture2D.MostDetailedMip	= 0;
			//desc.Texture2D.MipLevels		= 1;
 			//
			//
			//_RELEASE(impl.pView);
			//
			//ref_api_get_my_device()->pDevice->CreateShaderResourceView(texture->pTexture, &desc, &impl.pView);

			return true;
		}

		void api_release()
		{
			//_RELEASE(impl.pView);
		}

	};


	class gpu_sampler : public v_sampler         
	{
	private:
		impl_sampler impl;
	 
		GLint convert_sampler_mode(TextureSamplerMode mode)
		{
			switch (mode)
			{
			case TextureSamplerMode::Near:	return GL_NEAREST;
 			case TextureSamplerMode::Linear:		return GL_LINEAR;
 			case TextureSamplerMode::Anysotropic:	return GL_LINEAR; //todo
   			}
		}
		GLint convert_adress_mode(TextureAddressMode mode)
		{
			switch (mode)
			{
			case TextureAddressMode::Warp:		return GL_REPEAT;
 			case TextureAddressMode::Mirror:	return GL_MIRRORED_REPEAT;
 			case TextureAddressMode::Calm:		return GL_CLAMP_TO_EDGE;
 			case TextureAddressMode::Border:	return GL_CLAMP_TO_BORDER;
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
   		}






		void api_release()
		{
			//_RELEASE(impl.pState);
		}
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

		void api_release()
		{
			//_RELEASE(impl.pState);
		}
	};
	class gpu_color_blender : public v_color_blender         
	{
	private:
		impl_blender impl;
	protected:
		impl_blender* api_get_blender()
		{
			return &impl;
		}

		void api_release()
		{
			//_RELEASE(impl.pState);
		}
	};
	class gpu_topology : public v_topology        
	{
	private:
		impl_topology impl;
	protected:
		impl_topology* api_get_topology()
		{
			return &impl;
		}

		void api_release()
		{
			//_DELETE(impl.pState);
			//_RELEASE(impl.pState);
		}
	};

}
