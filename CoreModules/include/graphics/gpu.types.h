#pragma once

#include "../shared_in.h"

namespace graphics
{
	enum class TopologyMode
	{
		PointList,      //
		LineList,       //
		LineStrip,      //
		TriangleList,   //
		TriangleStrip,  //
		_enum_size,     //
	};

	enum class TextureSamplerMode
	{
		Near,                  //  Nearest pixel (recomendation: use 0.5 + pos)
		Linear,                //  Lenear interpolation
		Bilinear    = Linear,  //
		Trilinear   = Linear,  //
		Anysotropic,           //  Anysotropic filtration
		_enum_size,            //
	};

	enum class TextureAddressMode
	{
		Warp,        //  Repeate
		Mirror,      //  Repeate and flip outbounds
		Calm,        //  Trim to unorm
		Border,      //  Fill outbounds by color
		_enum_size,  //
	};

	enum class RasteriserMode
	{
		BothSides,   //
		InSide,      //
		OutSide,     //
		Wireframe,   //
		_enum_size,  //
	};

	enum class ColorBlendMode
	{
		Opaque,           //  Opaque texture
		AlphaMasked,      //  Opaque texture with alpha mask
		AlphaMaskedAtoC,  //  Opaque texture with alpha mask + alpha to coverage
		Translucent,      //  Translucent texture
		_enum_size,       //
	};

	enum class AntiAliasingMethod
	{
		NoAA,        //
		MSAA,        //  Over sampling anti-aliasing : [1-8] SampleLevels and [0-32] QualityLevels
		//SMAA,      //
		//FXAA,      //
		//TXAA,      //
		_enum_size,  //
	};

	enum class ShadingMode
	{
		Flat,        //  Flat shading  (aormals are perpendicular to the surface)
		Gouraud,     //  Guro shading  (averaging the normals of all adjacent edges)
		Phong,       //  Phong shading (ambient + diffuse + specular)
		//...        //
		//...        //
		//...        //
		_enum_size,  //
	};

	enum class DepthStencilClearMode
	{
		ClearDepth,    //  Clean depth buffer
		ClearStencil,  //  Clean stencil mask
		ClearFull,     //  Clean both
		_enum_size,    //
	};

	struct RenderState
	{
		bool FullScreen     = false;  //  [ false ]  Fullscreen application mode
		bool VSunc          = false;  //  [ true  ]  Vertical sync
		bool SlowOnUnFocuse = false;  //  [ true  ]  Decreased performance when focus losted
		bool IsFocused      = false;  //

		unsigned RealCanvasW   = 0;   //  [ 0 ]  Real canvas width  in pixels
		unsigned RealCanvasH   = 0;   //  [ 0 ]  Real canvas height in pixels
		unsigned RenderCanvasW = 0;   //  [ 0 ]  Virtual canvas width  in pixels
		unsigned RenderCanvasH = 0;   //  [ 0 ]  Virtual canvas height in pixels

 		AntiAliasingMethod AAMode = AntiAliasingMethod::NoAA;        //  [ NoAA ]        The method of smoothing the polygon frame
		TextureSamplerMode TSMode = TextureSamplerMode::Anysotropic; //  [ Anysotropic ] Texture filtering method(some objects can ignore this parameter)

		unsigned MSAA_SamplerCount  = 1;  //  [ 1 ]  MSAA samples count (from 1 to 8)
		unsigned MSAA_QualityLevel  = 0;  //  [ 0 ]  MSAA sampling quality (from 0 to 32 depended on samples num)
		unsigned AnisotropicQuality = 8;  //  [ 8 ]  Anisotropic texture filtering quality (from 1 to 16)

	public://-------------------------------------// mb dell
		os_window_handle WindowId;

		// need testes
		const bool single_thread_gpu_api_use_promise = false;
		const bool disable_gpu_task_timeout = false;

		//TODO
		const bool is_offscreen_render = false;
		const float real_screen_weight_in_centimeters = -1;
		const float real_screen_hight_in_centimeters = -1;

	protected:

		TopologyMode       _currentTopology   = (TopologyMode)-1;
		TextureSamplerMode _currentSampler    = (TextureSamplerMode)-1;
		RasteriserMode     _currentRasteriser = (RasteriserMode)-1;
		ColorBlendMode     _currentBlender    = (ColorBlendMode)-1;
	};

	struct RenderOptions : RenderState
	{
		os::Window* MyWindow = nullptr; // for debug, mb delete
		//os::Console* MyConsole;       // for debug, mb delete
	};

	enum class GpuAccess
	{
		unknown    = 0,
		no_access  = 0,
		read       = 2,
		write      = 4,
		read_write = read | write,
		_enum_size,
	};
	enum class GpuShaderType
	{
		undefined,
		vertex_shader,
		pixel_shader,
		//...
	};
	enum class GpuTextureType
	{
		texture_2d,
	//  texture_2d_ms,
	};
	enum class GpuBufferType
	{
		undefined,
		vertex_buffer,
		index_buffer,
		constant_buffer,
	};
	enum class GpuTexFormat
	{
		unknown,
		R8G8B8A8_UNORM, // narrow sampled float
	};

	struct TextureState
	{
		TextureState& operator=(const TextureState& target)
		{
			std::memcpy(this, &target, sizeof(target));
			return *this;
		}

		GpuTexFormat data_format = GpuTexFormat::R8G8B8A8_UNORM; // pixel format
		GpuAccess    cpu_access  = GpuAccess::no_access;		 // cpu access to object

		unsigned w = 0;
		unsigned h = 0;
		unsigned mip_levels = 0; //  how much mip-maps (0 = auto) shoud be created, manualy or by gpu_api_impl

		bool multisampled = false; // if multisampled, mip_levels shoud be 1
		bool ms_count     = 1;
		bool ms_quality   = 0;

		//todo
		const unsigned most_detaled_mip = 0; // base image is 0 mip level
		const unsigned array_size = 1;       // array-tex usage not expected
		const bool     bgra_mode  = false;
	};

	struct SamplerState
	{
	private:
		static SamplerState GenPreset(TextureSamplerMode tsm)
		{
			SamplerState out = SamplerState();
			out.FilterMIN = tsm;
			out.FilterMAG = tsm;
			out.FilterMIP = tsm;
			return out;
		}

	public:
		static SamplerState GetPresetPoint()   { return GenPreset(TextureSamplerMode::Near);        }
		static SamplerState GetPresetLinear()  { return GenPreset(TextureSamplerMode::Linear);      }
		static SamplerState GetPresetAnysotr() { return GenPreset(TextureSamplerMode::Anysotropic); }

		TextureSamplerMode FilterMIN  = TextureSamplerMode::Near;  //
		TextureSamplerMode FilterMAG  = TextureSamplerMode::Near;  //
		TextureSamplerMode FilterMIP  = TextureSamplerMode::Near;  //
		TextureAddressMode AdressMode = TextureAddressMode::Warp;  //

		// for mipmaps
		float MipBiasOffSet = 0.0f;  //  can by negative
		float MipMinLod     = 0.0f;  //  std::numeric_limits<float>::min();  //
		float MipMaxLod     = 0.0f;  //  std::numeric_limits<float>::max();  //  can be 0 for unstretchable (gui, ...)

		// for border filter
		FLOAT4 BorderColor = FLOAT4(255, 255, 255, 255);

		// for anysotropic filter
		unsigned AnysotropicMaxLevel = 16;

		// for multisampled textures
		bool Multisampled = false;
	};

#if (false) // most likely deprecated

	//=======================================================================================================//  lowlevel abstract gpu objects
	struct impl_factory;         struct v_factory;

	struct impl_adapter;         struct v_adapter;         //
	struct impl_output;          struct v_output;          //
	struct impl_device;          struct v_device;          //
	struct impl_swap_chain;      struct v_swap_chain;      //
	struct impl_texture;         struct v_texture;         //
	struct impl_buffer;          struct v_buffer;          //
	struct impl_render_target;   struct v_render_target;   //
	struct impl_depth_stencil;   struct v_depth_stencil;   //
	struct impl_shader;          struct v_shader;          //
	struct impl_shader_resourse; struct v_shader_resourse; //
	struct impl_sampler;         struct v_sampler;         //
	struct impl_rasterizer;      struct v_rasterizer;      //
	struct impl_blender;         struct v_color_blender;	 //
	struct impl_topology;        struct v_topology;        //

	/*class gpu_adapter;		*/
	/*class gpu_output;			*/
	/*class gpu_device;			*/
	/*class gpu_swap_chain;		*/
	/*							*/
	/*class gpu_texture;		*/
	/*class gpu_buffer;			*/
	/*class gpu_render_target;	*/
	/*class gpu_depth_stencil;	*/
	/*							*/
	/*class gpu_shader;			*/
	/*class gpu_shader_resourse;*/
	/*							*/
	/*class gpu_sampler;		*/
	/*class gpu_rasterizer;		*/
	/*class gpu_color_blender;	*/
	/*class gpu_topology;		*/

	//====================================================================//
	struct __gpu_factory_singleton
	{
		static v_factory* factory_singleton(v_factory* _fub = nullptr)
		{
			static v_factory* pFactorySingleton = nullptr;

			if (_fub)
			{
				if (pFactorySingleton)
				{
					log::error << "factory singleton already exist";
				}

				pFactorySingleton = _fub;
			}

			return pFactorySingleton;
		}
	};

	struct v_factory
	{
	protected:
		RenderState mRS;

	public:
		v_factory()
		{
			__gpu_factory_singleton::factory_singleton(this);
		}

		virtual ~v_factory()
		{

		}

		virtual const RenderState* get_rs() const
		{
			return &mRS;
		}

	public:
		virtual bool gpu_api_check_msaa_settings(size_t samples, size_t quality) = 0;

		virtual std::vector<impl_adapter*> enum_gpu_adapters() = 0;
		virtual std::vector<impl_output*>  enum_adapter_outputs(const impl_adapter*) = 0;

	protected:

		//---------------------------------------------------------------
		//  View / Output
		//---------------------------------------------------------------

		virtual bool gpu_api_change_view_port       () = 0;
		virtual bool gpu_api_change_v_sunc          (bool) = 0;
		virtual bool gpu_api_change_fullscreen_state(bool) = 0;

		virtual bool gpu_api_resize_render(size_t vw, size_t vh, size_t w, size_t h) = 0;
		virtual bool gpu_api_present_render() = 0;

		//---------------------------------------------------------------
		//  Render targets
		//---------------------------------------------------------------

		virtual void gpu_api_init_render_target (impl_render_target*) = 0;
		virtual void gpu_api_bind_render_target (impl_render_target*) = 0;
		virtual void gpu_api_free_render_target (impl_render_target*) = 0;

		virtual void gpu_api_init_depth_stencil (impl_depth_stencil*) = 0;
		virtual void gpu_api_bind_depth_stencil (impl_depth_stencil*) = 0;
		virtual void gpu_api_free_depth_stencil (impl_depth_stencil*) = 0;

		virtual void gpu_api_clean_output(impl_render_target*, FLOAT4,
			impl_depth_stencil*, DepthStencilClearMode) = 0;

		//---------------------------------------------------------------
		//  Draw calls
		//---------------------------------------------------------------

		virtual void gpu_api_draw(size_t point_count) = 0;
		virtual void gpu_api_draw_indexed(size_t index_count) = 0;
		virtual void gpu_api_draw_indexed_instansed(size_t index_count, size_t inst) = 0;

		//---------------------------------------------------------------
		//  Device
		//---------------------------------------------------------------

		virtual bool gpu_api_device_create  () = 0;
		virtual bool gpu_api_device_is_lost () = 0;
		virtual bool gpu_api_device_restart () = 0;

		//---------------------------------------------------------------
		//  Render state
		//---------------------------------------------------------------

		virtual void gpu_api_change_zbuffer_state(bool enabled) = 0;

		virtual void gpu_api_create_topology (impl_topology*   out, TopologyMode      ) = 0;
		virtual void gpu_api_create_sampler  (impl_sampler*    out, TextureSamplerMode) = 0;
		virtual void gpu_api_create_raster   (impl_rasterizer* out, RasteriserMode    ) = 0;
		virtual void gpu_api_create_blender  (impl_blender*    out, ColorBlendMode    ) = 0;

		virtual void gpu_api_bind_topology   (impl_topology*   ) = 0;
		virtual void gpu_api_bind_sampler    (impl_sampler*    ) = 0;
		virtual void gpu_api_bind_raster     (impl_rasterizer* ) = 0;
		virtual void gpu_api_bind_blender    (impl_blender*    ) = 0;

		virtual void gpu_api_free_topology   (impl_topology*   ) = 0;
		virtual void gpu_api_free_sampler    (impl_sampler*    ) = 0;
		virtual void gpu_api_free_raster     (impl_rasterizer* ) = 0;
		virtual void gpu_api_free_blender    (impl_blender*    ) = 0;

		//--------------------------------------------------------------------------// gpu_buffers

		// virtual bool gpu_api_generate_vbuffer(void* data, size_t data_size, bool cpu_acsess) = 0;   //  Create vertex   buffer ind map it to gpu memory
		// virtual bool gpu_api_generate_ibuffer(void* data, size_t data_size, bool cpu_acsess) = 0;   //  Create index    buffer ind map it to gpu memory
		// virtual bool gpu_api_generate_cbuffer(void* data, size_t data_size, bool cpu_acsess) = 0;   //  Create constant buffer ind map it to gpu memory
		//
		// virtual void gpu_api_set_vertex_buffer(api_impl_buffer) = 0;
		// virtual void gpu_api_set_index_buffer(api_impl_buffer) = 0;
		// virtual void gpu_api_set_const_buffer(api_impl_buffer) = 0;
		//
		// //--------------------------------------------------------------------------//  gpu_texture
		//
		// virtual bool gpu_api_texture_from_file(const std::string& utf8_path) = 0;
		//
		// //--------------------------------------------------------------------------//  gpu_shader
		//
		// virtual bool gpu_api_compilite_vertex_shader(const std::string& asci_code, const api_impl_shader_initdata& sid) = 0;
		// virtual bool gpu_api_compilite_pixel_shader (const std::string& asci_code, const api_impl_shader_initdata& sid) = 0;
	};

	//////////////////////////////////////////////////////////////////////////

	struct factory_ref
	{
	protected:
		std::string ref_debug_name;

	public:
		void Release()
		{
			api_release();
		}

	protected:
		const impl_factory* api_get_factory()
		{
			return (impl_factory*)__gpu_factory_singleton::factory_singleton(nullptr);
		}

		void api_set_my_debug_name(const std::string& deb_name)
		{
			ref_debug_name = deb_name;
		}

		std::string api_get_my_debug_name()
		{
			return ref_debug_name;
		}

		virtual void    api_release    () { }
		virtual int64_t api_get_memsize() { return -1; }
	};


	struct v_adapter : factory_ref
	{
	public:
		struct adapter_info
		{
			std::string description  = "";
			std::string adapter_luid = "";

			uint32_t vendor_id = 0;
			uint32_t device_id = 0;
			uint32_t subsys_id = 0;
			uint32_t revision  = 0;

			int64_t  dedicated_video_mem;  //  Video card own memory | fastest |  CPU access: [-]
			int64_t  dedicated_sys_mem;    //  System lended memory  | slow    |  CPU access: [-]
			int64_t  shared_system_mem;    //  System shared memory  | slowest |  CPU access: [+]
		};

		const impl_adapter* GetAdapter()
		{
			return api_get_adapter();
		}

		adapter_info GetAdapterInfo()
		{
			return api_get_info();
		}

	protected:
		virtual impl_adapter* api_get_adapter() = 0;
		virtual adapter_info  api_get_info() = 0;

	};

	struct v_output : factory_ref
	{
	public:
		struct output_info
		{
			std::string description;
			bool  os_AttachedToDesktop;
			QUAD  os_Coord;
			float os_Rotation;
			void* os_hMonitor;
		};
		struct output_mode
		{
			unsigned Width;
			unsigned Height;
			unsigned RefreshNum;
			unsigned RefreshDen;

			//  DXGI_FORMAT Format;
			//  DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
			//  DXGI_MODE_SCALING Scaling;
		};

		const impl_output* GetOutput()
		{
			return api_get_output();
		}

		output_info GetOutputInfo()
		{
			return api_get_output_info();
		}

		std::vector<output_mode> GetOutputModes()
		{
			return api_get_output_modes();
		}

	protected:
		virtual impl_output*             api_get_output() = 0;
		virtual output_info              api_get_output_info() = 0;
		virtual std::vector<output_mode> api_get_output_modes() = 0;
	};

	struct v_device : factory_ref
	{
	public:
		impl_device* GetDevice() { return api_get_device(); }

	protected:
		virtual impl_device* api_get_device() = 0;
	};

	struct v_swap_chain : factory_ref
	{
	public:
		const impl_swap_chain* GetSwapChain() { return api_get_swap_chain(); }

	protected:
		virtual const impl_swap_chain* api_get_swap_chain() = 0;
	};

	struct v_texture : factory_ref
	{
	public:
		virtual bool api_check_state(const texture_state& state)
		{
			assert(state.w > 0);
			assert(state.h > 0);
			assert(state.mip_levels >= 0);

			if (state.multisampled == true)
			{
				assert(state.mip_levels == 1);
				assert(state.multisample_count   >= 1 && state.multisample_count   <= 8);
				assert(state.multisample_quality >= 1 && state.multisample_quality <= 33);

				//ref_api_get_my_fubric()->gpu_api_check_msaa_settings(state.multisample_count, state.multisample_quality);
			}

			return true;
		}

		virtual bool api_check_compatibility(const texture_state& stateA, const texture_state& stateB)
		{
			bool comp = true;

			comp &= (stateA.w == stateB.w);
			comp &= (stateA.h == stateB.h);
			comp &= (stateA.data_format  == stateB.data_format);
			comp &= (stateA.multisampled == stateB.multisampled);

			if (stateA.multisampled)
			{
				comp &= (stateA.multisample_count   == stateB.multisample_count);
				comp &= (stateA.multisample_quality == stateB.multisample_quality);
			}

			return comp;
		}

	public:
		texture_state GetState  () { return api_get_state(); }
		impl_texture* GetTexture() { return api_get_impl();  }

		bool CreateTex(texture_state opt, void* init_data = nullptr)
		{
			api_check_state(opt);

			return api_create(opt, init_data);
		}

		bool Bind(size_t slot) { return api_bind(slot); }

		bool UpdateData(void* data) { return api_update_data(data); }

		bool UpdateSubRegionData(const QUAD& rect, void* data) { return api_update_sub_data(rect, data); }

		bool Resize(unsigned w, unsigned h) { return api_resize(w, h); }

		bool CopyDataTo(v_texture& target)
		{
			texture_state t1_state = this->api_get_state();
			texture_state t2_state = target.api_get_state();

			if (false == v_texture::api_check_compatibility(t1_state, t2_state))
				return false;

			return api_copy_tex_all_to((impl_texture*)target.api_get_impl());
		}

		bool CopySubRegionTo(v_texture& target, const QUAD& Scr, const QUAD& Dest)
		{
			texture_state t1_state = this->api_get_state();
			texture_state t2_state = target.api_get_state();

			if (false == v_texture::api_check_compatibility(t1_state, t2_state))
				return false;

			return api_copy_tex_sub_to((impl_texture*)target.api_get_impl(), Scr, Dest);
		}

	protected:
		virtual impl_texture* api_get_impl() = 0;

		virtual texture_state api_get_state() = 0;

		virtual bool api_create(texture_state opt, void* init_data) = 0;

		virtual bool api_bind(size_t slot) = 0;

		virtual void api_unbind() = 0;

		virtual bool api_update_data(void* data) = 0;

		virtual bool api_update_sub_data(const QUAD& rect, void* data) = 0;

		virtual bool api_resize(unsigned, unsigned) = 0;

		virtual bool api_copy_tex_all_to(impl_texture* target)
		{
			LOG_WARN_NO_IMPL;
			return false;
		}

		virtual bool api_copy_tex_sub_to(impl_texture* target, const QUAD& boxScr, const QUAD& boxDest)
		{
			LOG_WARN_NO_IMPL;
			return false;
		}
	};

	struct v_buffer : factory_ref
	{
	public:
		impl_buffer* GetBuffer() { return api_get_buffer(); }

		bool Create(GpuBufferType type, void* data, size_t data_size, size_t item_size, GpuAccess cpu_access)
		{
			return gpu_api_generate_buffer(type, data, data_size, item_size, cpu_access);
		}

	protected:
		virtual bool gpu_api_generate_buffer(GpuBufferType type, void* data, size_t data_size, size_t item_size, GpuAccess cpu_access)
		{
			LOG_WARN_NO_IMPL;
			return false;
		}

		virtual impl_buffer* api_get_buffer()
		{
			LOG_WARN_NO_IMPL;
			return nullptr;
		}
	};

	struct v_render_target : factory_ref
	{
	public:
		impl_render_target* GetRenderTarget()
		{
			return api_get_render_target();
		}

	protected:
		virtual impl_render_target* api_get_render_target() = 0;
	};

	struct v_depth_stencil : factory_ref
	{
	public:
		impl_depth_stencil* GetDepthStencil()
		{
			return api_get_depth_stencil();
		}

	protected:

		virtual impl_depth_stencil* api_get_depth_stencil() = 0;
	};

	struct v_shader : factory_ref
	{
	public:
		bool Compilite(GpuShaderType type, const std::string& asci_code, const int semantic)
		{
			return api_compilite(type, asci_code, semantic);
		}

		impl_shader* GetShader()
		{
			return api_get_shader();
		}

	protected:
		virtual impl_shader* api_get_shader() = 0;

		virtual bool api_compilite(GpuShaderType type, const std::string& asci_code, const int semantic) = 0;

	};
	struct v_shader_resourse : factory_ref
	{
	public:
		impl_shader_resourse* GetShaderResourse()
		{
			return api_get_shader_resourse();
		}

		bool CreateShaderResourse(impl_texture* texture)
		{
			return api_get_create_shader_resourse(texture);
		}


	protected:

		virtual impl_shader_resourse* api_get_shader_resourse() = 0;
		virtual bool api_get_create_shader_resourse(impl_texture*) = 0;
	};

	//==============================================// abstract sampler api

	struct v_sampler
	{

	public:
		static sampler_opt GetPointPreset()
		{
			sampler_opt out;

			out.FilterMIN = TextureSamplerMode::Near;
			out.FilterMAG = TextureSamplerMode::Near;
			out.FilterMIP = TextureSamplerMode::Near;

			return out;
		}

		static sampler_opt GetLinearPreset()
		{
			sampler_opt out;

			out.FilterMIN = TextureSamplerMode::Linear;
			out.FilterMAG = TextureSamplerMode::Linear;
			out.FilterMIP = TextureSamplerMode::Linear;

			return out;
		}

		static sampler_opt GetAnysotropicPreset()
		{
			sampler_opt out;

			out.FilterMIN = TextureSamplerMode::Anysotropic;
			out.FilterMAG = TextureSamplerMode::Anysotropic;
			out.FilterMIP = TextureSamplerMode::Anysotropic;

			return out;
		}

	public:
		impl_sampler* GetSampler() { return api_get_sampler(); }

		bool Create(sampler_opt opt = GetPointPreset()) { return api_create(opt); }
		bool Bind  (unsigned slot = 0)                    { return api_bind(slot); }

	protected:
		virtual bool api_create (sampler_opt opt) = 0;
		virtual bool api_bind   (unsigned slot) = 0;

		virtual impl_sampler* api_get_sampler() = 0;
		virtual sampler_opt*  api_get_binded_sampler()
		{
			LOG_WARN_NO_IMPL;
			return false;
		}
	};

	struct v_rasterizer
	{
	public:             impl_rasterizer* GetRasterizer     () { return api_get_rasterizer();    }
	protected:  virtual impl_rasterizer* api_get_rasterizer() { LOG_WARN_NO_IMPL; return nullptr; }
	};

	struct v_color_blender
	{
	public:            impl_blender* GetBlender     () { return api_get_blender();       }
	protected: virtual impl_blender* api_get_blender() { LOG_WARN_NO_IMPL; return nullptr; }
	};

	struct v_topology
	{
	public:            impl_topology* GetTopology     () { return api_get_topology();      }
	protected: virtual impl_topology* api_get_topology() { LOG_WARN_NO_IMPL; return nullptr; }
	};

#endif

}



