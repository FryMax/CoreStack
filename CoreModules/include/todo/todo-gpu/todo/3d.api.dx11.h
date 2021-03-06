#pragma once

#include "../shared_in.h"
 
DEF_UMODULE( module_dx11, "DirectX library",
"DX: "		+ std::to_string(D3D11_MAJOR_VERSION)		 + "." + std::to_string(D3D11_MINOR_VERSION)		+ " " +
"SHADER: "	+ std::to_string(D3D11_SHADER_MAJOR_VERSION) + "." + std::to_string(D3D11_SHADER_MINOR_VERSION)	+ " " +
"SDK: "		+ std::to_string(D3D11_SDK_VERSION), "");

//////////////////////////////////////////////////////////////////////////


struct gpu_fubric_api_dx11 : gpu_fubric_api
{
public:
    __ffast ID3D11Device*           DX11GetDevice()         { return v_device.GetDevice()->pDevice;           }
    __ffast ID3D11DeviceContext*    DX11GetDeviceContext()  { return v_device.GetDevice()->pContext;          }
    __ffast IDXGISwapChain4*        DX11SwapChain()         { return v_swap_chain.GetSwapChain()->pSwapchain; }
    __ffast os_window_handle		GetWindowHwnd()         { return mRS.WindowHwnd;						 }

private:
    IDXGIFactory5 *     _dx_pFactory = nullptr;
    D3D_DRIVER_TYPE     _dx_DriverType;
    D3D_FEATURE_LEVEL   _dx_FeautureLevel;

 
	bool DX11CheckMSAASettings(impl_device* dev, UINT Samples, UINT Qality, bool NeedReport = true)
	{
		RESULT OK;
        UINT SamplerMaxQ = 0;

        assert(dev->pDevice);
 
		if (false == math::InRange(Samples, 1U, 8U))  return false;
		if (false == math::InRange(Qality,  0U, 32U)) return false;
		 
		OK << dev->pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, Samples, &SamplerMaxQ);
 
        if (OK.IsSucceed()) 
        {
            if (true == math::InRange(Qality, 0U, SamplerMaxQ--))
                return true;
            else 
                if (NeedReport)
                {
                    std::string sms = fmt::format("MSAA options error : S/Q [{}/{}]. MaxQ : [{}]. AA will not be enabled", Samples, Qality, SamplerMaxQ);
					log::debug << (sms);
                }
        }

        return false;
	}

    bool ResetRenderTarget(size_t w, size_t h) 
    {

        _main_rt.Release();
        _main_ds.Release();
        _main_rt_texture.Release();
        _main_ds_texture.Release();



        DX11GetDeviceContext()->ClearState();
        DX11GetDeviceContext()->OMSetRenderTargets(0, 0, 0);

        RESULT OK;

		OK << DX11SwapChain()->ResizeBuffers(0, w, h, DXGI_FORMAT_R8G8B8A8_UNORM/*DXGI_FORMAT_UNKNOWN*/, 0);
  		OK << DX11SwapChain()->GetBuffer(0, __uuidof (ID3D11Texture2D), (void**) &_main_rt_texture.GetTexture()->pTexture);
		OK << DX11GetDevice()->CreateRenderTargetView(_main_rt_texture.GetTexture()->pTexture, NULL, &_main_rt.GetRenderTarget()->pTarget);
		//OK << pDevice->CreateShaderResourceView(pBackBuffer, &desc, &pDS_ShaderView);
 		
 		//////////////////////////////////////////////////////////////////////////
 		D3D11_TEXTURE2D_DESC			TexDesc	 = {};
		D3D11_DEPTH_STENCIL_VIEW_DESC	ViewDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC ShadDesk = {};
 
		TexDesc.Width				= w;
		TexDesc.Height				= h;
		TexDesc.MipLevels			= 1;
		TexDesc.ArraySize			= 1;
		TexDesc.MiscFlags			= 0;
		TexDesc.CPUAccessFlags		= 0;
		TexDesc.SampleDesc.Count	= mRS.MSAA_SamplerCount;
		TexDesc.SampleDesc.Quality	= mRS.MSAA_QualityLevel;
		TexDesc.Usage				= D3D11_USAGE_DEFAULT;
		TexDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		TexDesc.Format				= DXGI_FORMAT_R32_TYPELESS;
		//---------------------------------------------------------
		ViewDesc.Flags				= 0;
 		ViewDesc.Texture2D.MipSlice = 0;
		ViewDesc.Format				= DXGI_FORMAT_D32_FLOAT;
		ViewDesc.ViewDimension		= (mRS.MSAA_SamplerCount < 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
		//---------------------------------------------------------
  		ShadDesk.Format						= DXGI_FORMAT_R32_FLOAT;
		ShadDesk.ViewDimension				= (mRS.MSAA_SamplerCount < 1) ?  D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
		ShadDesk.Texture2D.MostDetailedMip	= 0;
		ShadDesk.Texture2D.MipLevels		= 1;
 
 		OK << DX11GetDevice()->CreateTexture2D          (&TexDesc, NULL, &_main_ds_texture.GetTexture()->pTexture);
		OK << DX11GetDevice()->CreateDepthStencilView   (_main_ds_texture.GetTexture()->pTexture, &ViewDesc, &_main_ds.GetDepthStencil()->pDepth);
		//OK << _device.impl.pDevice->CreateShaderResourceView(_main_ds.depth_stencil_buffer.impl.texture, &ShadDesk, &_main_depth_stencil_shader_view.impl.view);

        return OK.IsSucceed();
    }


public:
    IMPL_CALL std::vector<gpu_adapter> enum_gpu_adapters()
    {
        assert(_dx_pFactory != nullptr);

        std::vector<gpu_adapter> out_adapters;
        /////////////////////////////////////
		IDXGIAdapter *pAdapter = nullptr;
		int i = 0;
		while (_dx_pFactory->EnumAdapters(i++, &pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			IDXGIAdapter3* pAdapter_newest;
			pAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter_newest);

			gpu_adapter adapter;
			((impl_adapter*)adapter.GetAdapter())->pAdapter = pAdapter_newest;

			out_adapters.push_back(adapter);
		}
		/////////////////////////////////////

        out_adapters.shrink_to_fit();
        return out_adapters;
    }
 	IMPL_CALL std::vector<gpu_output>  enum_adapter_outputs(gpu_adapter* pAdp) 
	{
		assert(pAdp);

		IDXGIAdapter *pAdapter = pAdp->GetAdapter()->pAdapter;
		std::vector<gpu_output> out_adap_outputs;
		/////////////////////////////////////
		IDXGIOutput* pOutput;
 		int i = 0;
 		while (pAdapter->EnumOutputs(i++, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
  			gpu_output output;
			((impl_output*)output.GetOutput())->pOutput = pOutput;
			out_adap_outputs.push_back(output);
 		}
		/////////////////////////////////////
		out_adap_outputs.shrink_to_fit();
		return out_adap_outputs;
	}



    //--------------------------------------------------------------------------// Render

    IMPL_CALL bool gpu_api_present_render()
    {

        // todo vsunc work

        DX11SwapChain()->Present(0, 0);

        return this;
    }

    IMPL_CALL bool gpu_api_resize_render(size_t vw, size_t vh, size_t w, size_t h)
	{
 		if (ResetRenderTarget(vw, vh))
		{
  			mRS.RenderCanvasWight	= vw;
			mRS.RenderCanvasHeight	= vh;
			mRS.RealCanvasWeight	= w;
			mRS.RealCanvasHight		= h;

 			D3D11_VIEWPORT vp;
			vp.Width = w;
			vp.Height = h;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			DX11GetDeviceContext()->RSSetViewports(1, &vp);

			return true;
		}

		return false;
	}

    IMPL_CALL bool gpu_api_set_fullscreen_state (bool state)
    {
        RESULT OK;
        BOOL fullscr = state ? TRUE : FALSE;
        //////////////////////////////////////////////////////////////////////////
        OK << DX11SwapChain()->SetFullscreenState(fullscr, nullptr);
        OK << DX11SwapChain()->GetFullscreenState(&fullscr, nullptr);
        //////////////////////////////////////////////////////////////////////////
        mRS.FullScreen = (state == TRUE);
        return mRS.FullScreen == state;
    }
    IMPL_CALL bool gpu_api_get_fullscreen_state ()
    {
        RESULT OK;
        BOOL state = FALSE;
        //////////////////////////////////////////////////////////////////////////
        OK << DX11SwapChain()->GetFullscreenState(&state, nullptr);
        //////////////////////////////////////////////////////////////////////////
        mRS.FullScreen = (state == TRUE);
        return mRS.FullScreen;
    }

    //--------------------------------------------------------------------------// render targets

    IMPL_CALL void gpu_api_bind_render_targets (impl_render_target* rt, impl_depth_stencil* ds) { DX11GetDeviceContext()->OMSetRenderTargets(1, &rt->pTarget, ds->pDepth);        }
    IMPL_CALL void gpu_api_clear_render_target (FLOAT4 color, impl_render_target* target)       { DX11GetDeviceContext()->ClearRenderTargetView(target->pTarget, &color.x);   }
    IMPL_CALL void gpu_api_clear_depth_stencil (DSClearMode mode, impl_depth_stencil* target) 
    {       
        
        UINT clear_mode = 0;
        switch (mode)
        {
        case DSClearMode::ClearDepth:	clear_mode = D3D11_CLEAR_DEPTH;		break;
        case DSClearMode::ClearStencil:	clear_mode = D3D11_CLEAR_STENCIL;	break;
        case DSClearMode::ClearFull:	clear_mode = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;	break;
        }

        DX11GetDeviceContext()->ClearDepthStencilView(target->pDepth, clear_mode, 1, 0);

    }

    //--------------------------------------------------------------------------// draw calls

    IMPL_CALL void gpu_api_draw        (size_t point_count) { DX11GetDeviceContext()->Draw(point_count, 0);            }
    IMPL_CALL void gpu_api_draw_indexed(size_t index_count) { DX11GetDeviceContext()->DrawIndexed(index_count, 0, 0);  }

    //--------------------------------------------------------------------------// init

    IMPL_CALL bool gpu_api_init_device      ()
	{
 		RESULT OK;

  		D3D_DRIVER_TYPE	  DriverTypes  [] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_SOFTWARE, D3D_DRIVER_TYPE_UNKNOWN };
		D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, };
		constexpr UINT	  DT_count	= ARRAY_SIZE(DriverTypes);
		UINT			  FL_count	= ARRAY_SIZE(FeatureLevels);
  		////////////////////////////////////////////////////////////////////////// flags
		UINT DeviceFlags = 0;

#ifdef _DEBUG		
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	//	DeviceFlags |= D3D11_CREATE_DEVICE_DEBUGGABLE;  // [Direct3D 11.1]	// D3D_DRIVER_TYPE_WARP
#endif
        if (mRS.single_thread_gpu_api_use_promise)  DeviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
        if (mRS.disable_gpu_task_timeout)           DeviceFlags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT; // [Direct3D 11.1]
 
 		//////////////////////////////////////////////////////////////////////////

        OK << CreateDXGIFactory2(DeviceFlags & D3D11_CREATE_DEVICE_DEBUG ? D3D11_CREATE_DEVICE_DEBUG : 0, __uuidof(IDXGIFactory5), (void**)(&_dx_pFactory));

		//auto _adapter = enum_gpu_adapters().at(0);
		//auto _output  = enum_adapter_outputs(&_adapter).at(0);

 		if ("Create Device")
		{
 			ForArray(i, DT_count)
			{
 				D3D_DRIVER_TYPE		DriverType	= DriverTypes[i];
                D3D_FEATURE_LEVEL	feature_level;

                IDXGIAdapter3		*adapter = nullptr;	// used gpu device
                ID3D11Device        *device	 = nullptr;
                ID3D11DeviceContext *context = nullptr;
 
				 
				OK << ::D3D11CreateDevice(adapter, DriverType, NULL, DeviceFlags, FeatureLevels, FL_count, D3D11_SDK_VERSION,
					&device,
					&feature_level,
					&context);
 
				if (OK.IsSucceed()) 
				{
					device->QueryInterface (__uuidof (ID3D11Device1),		 (void **)&v_device.GetDevice()->pDevice);
					context->QueryInterface(__uuidof (ID3D11DeviceContext1), (void **)&v_device.GetDevice()->pContext);

                    _dx_DriverType    = DriverType;
                    _dx_FeautureLevel = feature_level;
		
 					break;
				}
  			}

			if (OK.IsFailed())
				return false;

			if ("Debug")
			{
 
			}
   		}
        //////////////////////////////////////////////////////////////////////////
 		if ("SwapChain")
		{
			DXGI_SWAP_CHAIN_DESC1			 SCD = {};
			DXGI_SWAP_CHAIN_FULLSCREEN_DESC	FSCD = {};

 			switch (mRS.AAMode)
			{
			case AntiAliasingMode::NoAA:
 				
				mRS.MSAA_SamplerCount = 1;
				mRS.MSAA_QualityLevel = 0;
 				break;

 			case AntiAliasingMode::MSAA:
 				
				bool msaa_test = DX11CheckMSAASettings(v_device.GetDevice(), mRS.MSAA_SamplerCount, mRS.MSAA_QualityLevel, true);
				if (msaa_test == false)
				{
					mRS.AAMode = AntiAliasingMode::NoAA;
					mRS.MSAA_SamplerCount = 1;
					mRS.MSAA_QualityLevel = 0;
				}
				break;
			 
 			}
 
            log::debug << fmt::format("msaa: {} {}", mRS.MSAA_SamplerCount, mRS.MSAA_QualityLevel);
			//////////////////////////////////////////////////////////////////////////
			//size_t RefN				= disp_modes[disp_modes.size() - 1].RefN;
			//size_t RefD				= disp_modes[disp_modes.size() - 1].RefD;
			//_RS.RealCanvasWeight		= 800;
			//_RS.RealCanvasHight		= 600;
			//_RS.VirtualCanvasWight	= 800;
			//_RS.VirtualCanvasHeight	= 600;
 
		    //=====================================================================// SwapChain mode
 			SCD.Width				= mRS.RenderCanvasWight;
			SCD.Height				= mRS.RenderCanvasHeight;
			SCD.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
			SCD.Stereo				= FALSE;
			SCD.SampleDesc.Count	= mRS.MSAA_SamplerCount;
			SCD.SampleDesc.Quality	= mRS.MSAA_QualityLevel;
			SCD.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SCD.BufferCount			= 2;
			SCD.Scaling				= DXGI_SCALING_STRETCH;//DXGI_SCALING_STRETCH;
            SCD.SwapEffect          = DXGI_SWAP_EFFECT_FLIP_DISCARD;// ;
            SCD.AlphaMode           = DXGI_ALPHA_MODE_UNSPECIFIED;// ;
			SCD.Flags				= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		    //=====================================================================// Fullscreen mode state
			//FSCD.RefreshRate.Numerator   = RefN;
			//FSCD.RefreshRate.Denominator = RefD;
			FSCD.ScanlineOrdering		 = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			FSCD.Scaling				 = DXGI_MODE_SCALING_UNSPECIFIED;
			FSCD.Windowed				 = !mRS.FullScreen;
			 
			//////////////////////////////////////////////////////////////////////////
  			//	IDXGIDevice*	dxgiDevice = 0;
  			//	IDXGIAdapter*	dxgiAdapter = 0;
  			//	IDXGIFactory*	dxgiFactory = 0;
  			//	
  			//	OK << pDevice->QueryInterface(__uuidof(IDXGIDevice),  (void**)&dxgiDevice);		 
  			//	OK << dxgiDevice->GetParent	 (__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);	 
  			//	OK << dxgiAdapter->GetParent (__uuidof(IDXGIFactory), (void**)&dxgiFactory);	 
 			//	OK << dxgiFactory->CreateSwapChain(pDevice, &SCD, &pSwapChain);					 
			//	OK << dxgiFactory->MakeWindowAssociation(mRS.WindowHwnd, DXGI_MWA_NO_ALT_ENTER);
			//
			//	_RELEASE(dxgiDevice);
			//	_RELEASE(dxgiAdapter);
			//	_RELEASE(dxgiFactory);
            IDXGISwapChain1* pSwapChain;
 
 			OK << _dx_pFactory->CreateSwapChainForHwnd(v_device.GetDevice()->pDevice, mRS.WindowHwnd.hwnd, &SCD, &FSCD, NULL, &pSwapChain);
			OK << _dx_pFactory->MakeWindowAssociation(NULL, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
            OK << pSwapChain->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&v_swap_chain.GetSwapChain()->pSwapchain);

            log::debug << "SwapChain";
 		}
	

        if ("BackBuffer DepthStencil Buffer and Shader")
        {

            OK << ResetRenderTarget(mRS.RenderCanvasHeight, mRS.RenderCanvasWight);

        }
            
 
 
// 			D3D11_DEPTH_STENCIL_DESC DSD = {};
// 
//  		DSD.DepthEnable						= true;
// 			DSD.StencilEnable					= true;
// 			DSD.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
// 			DSD.DepthFunc						= D3D11_COMPARISON_LESS;
// 			DSD.StencilReadMask					= D3D11_DEFAULT_STENCIL_READ_MASK;		// 0xFF;
// 			DSD.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;		// 0xFF;
// 			DSD.FrontFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;				//	FrontFace STENCIL_OP_KEEP
// 			DSD.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;				//	FrontFace STENCIL_OP_INCR  (INCR)
// 			DSD.FrontFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;				//	FrontFace STENCIL_OP_KEEP
// 			DSD.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;				//	FrontFace COMPARISON_ALWAYS
// 			DSD.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;				//	BackFace  STENCIL_OP_KEEP
// 			DSD.BackFace.StencilDepthFailOp		= D3D11_STENCIL_OP_DECR;				//	BackFace  STENCIL_OP_DECR  (DECR)
// 			DSD.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;				//	BackFace  STENCIL_OP_KEEP
// 			DSD.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;				//	BackFace  COMPARISON_ALWAYS
// 
////////////////////////////////////////////////////////////////////////////
// 
// 			OK = pDevice->CreateDepthStencilState(&DSD, &r_Depth_Stencil_State_Z_ON);  OK == ("CreateDepthStencilView error");	// � ��������
// 
// 			DSD.DepthEnable = false;
// 
// 			OK = pDevice->CreateDepthStencilState(&DSD, &r_Depth_Stencil_State_Z_OFF); OK == ("CreateDepthStencilView error");		// ��� �������
   
		return true;
	}
    IMPL_CALL bool gpu_api_init_topologys   () 	 //	init topologys	
    {
        if (!v_device.GetDevice()->pDevice)
            return false;
 
        _topologys[(size_t)TopologyMode::PointList].GetTopology()->pState       = new D3D_PRIMITIVE_TOPOLOGY(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
        _topologys[(size_t)TopologyMode::LineList].GetTopology()->pState        = new D3D_PRIMITIVE_TOPOLOGY(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        _topologys[(size_t)TopologyMode::LineStrip].GetTopology()->pState       = new D3D_PRIMITIVE_TOPOLOGY(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
        _topologys[(size_t)TopologyMode::TriangleList].GetTopology()->pState    = new D3D_PRIMITIVE_TOPOLOGY(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _topologys[(size_t)TopologyMode::TriangleStrip].GetTopology()->pState   = new D3D_PRIMITIVE_TOPOLOGY(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        return true;
    }
    IMPL_CALL bool gpu_api_init_samplers    ()
    {
        if (!v_device.GetDevice()->pDevice)
            return false;

		D3D11_SAMPLER_DESC SD = {};
 
		SD.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
		SD.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
		SD.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
 		SD.ComparisonFunc	= D3D11_COMPARISON_NEVER;
 		SD.MinLOD			= -FLT_MAX;
		SD.MaxLOD			= +FLT_MAX;
		SD.MaxAnisotropy	= mRS.AnisotropicQuality;
 		SD.MipLODBias		= 0.0f;
		SD.BorderColor[0]	= 1.0f;
		SD.BorderColor[1]	= 1.0f;
		SD.BorderColor[2]	= 1.0f;
		SD.BorderColor[3]	= 1.0f;
        //////////////////////////////////////////////////////////////////////////
		D3D11_SAMPLER_DESC NearPix	   (SD);
		D3D11_SAMPLER_DESC Bilinear	   (SD);
		D3D11_SAMPLER_DESC Trilinear   (SD);
		D3D11_SAMPLER_DESC Anysotropic (SD);
 
 		NearPix.	Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		Bilinear.	Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		Trilinear.	Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
 		Anysotropic.Filter = D3D11_FILTER_ANISOTROPIC;

		RESULT OK;

 		OK << v_device.GetDevice()->pDevice->CreateSamplerState(&NearPix,     &_samplers[(size_t)SamplerMode::NearPixel].GetSampler()->pState);	
  		OK << v_device.GetDevice()->pDevice->CreateSamplerState(&Bilinear,    &_samplers[(size_t)SamplerMode::Bilinear].GetSampler()->pState);	
  		OK << v_device.GetDevice()->pDevice->CreateSamplerState(&Trilinear,   &_samplers[(size_t)SamplerMode::Trilinear].GetSampler()->pState);	
 		OK << v_device.GetDevice()->pDevice->CreateSamplerState(&Anysotropic, &_samplers[(size_t)SamplerMode::Anysotropic].GetSampler()->pState);	

		return OK.IsSucceed();
	}
    IMPL_CALL bool gpu_api_init_rasterizers ()
    {
        if (!v_device.GetDevice()->pDevice)
            return false;

		D3D11_RASTERIZER_DESC RD = {};	// Default

 		RD.DepthBias			 = 0;
		RD.SlopeScaledDepthBias  = 0.0f;
		RD.DepthBiasClamp		 = 0.0f;
		RD.DepthClipEnable		 = true;
		RD.ScissorEnable		 = false;
		RD.MultisampleEnable	 = mRS.MSAA_SamplerCount > 1 ? true : false;
		RD.AntialiasedLineEnable = false; // ������� ����� �����
		RD.FrontCounterClockwise = false;
        //////////////////////////////////////////////////////////////////////////
		D3D11_RASTERIZER_DESC RD_WIRE(RD);
		D3D11_RASTERIZER_DESC RD_ALL (RD);
		D3D11_RASTERIZER_DESC RD_IN  (RD);
		D3D11_RASTERIZER_DESC RD_OUT (RD);

		RD_WIRE.FillMode	= D3D11_FILL_WIREFRAME;
		RD_ALL.FillMode		= D3D11_FILL_SOLID;
		RD_IN.FillMode		= D3D11_FILL_SOLID;
		RD_OUT.FillMode		= D3D11_FILL_SOLID;

		RD_WIRE.CullMode	= D3D11_CULL_NONE;
		RD_ALL.CullMode		= D3D11_CULL_NONE;
		RD_IN.CullMode		= D3D11_CULL_FRONT;
		RD_OUT.CullMode		= D3D11_CULL_BACK;

		RESULT OK;

		OK << v_device.GetDevice()->pDevice->CreateRasterizerState(&RD_WIRE,	&_rasterizers[(size_t)RasteriserMode::Wireframe].GetRasterizer()->pState);
		OK << v_device.GetDevice()->pDevice->CreateRasterizerState(&RD_ALL,	&_rasterizers[(size_t)RasteriserMode::BothSides].GetRasterizer()->pState);
		OK << v_device.GetDevice()->pDevice->CreateRasterizerState(&RD_IN,	&_rasterizers[(size_t)RasteriserMode::InSide].GetRasterizer()->pState);
		OK << v_device.GetDevice()->pDevice->CreateRasterizerState(&RD_OUT,	&_rasterizers[(size_t)RasteriserMode::OutSide].GetRasterizer()->pState);
	
		return OK.IsSucceed();
    }
    IMPL_CALL bool gpu_api_init_blenders    ()
    {
        if (!v_device.GetDevice()->pDevice)
            return false;

        D3D11_BLEND_DESC BD = {};

        BD.AlphaToCoverageEnable  = FALSE;	// �������������� ��������������� �������������� ��������� ���� ������ ������ ��� �����
        BD.IndependentBlendEnable = FALSE;	// ���� FALSE �� ������������ ������ 1 ������� ������� ���������� ����������, ��������� 7 ������������

        BD.RenderTarget[0].BlendEnable           = FALSE;
        BD.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
        BD.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
        BD.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
        BD.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
        BD.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
        BD.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        BD.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        //////////////////////////////////////////////////////////////////////////
		D3D11_BLEND_DESC BD_Opaque		 (BD);
		D3D11_BLEND_DESC BD_AlphaMask	 (BD);
		D3D11_BLEND_DESC BD_AlphaMaskAtoC(BD);
        D3D11_BLEND_DESC BD_Transperent	 (BD);

		//-------------------------------------------------------// Opaque
		BD_Opaque.RenderTarget[0].BlendEnable = FALSE;
		//-------------------------------------------------------// AlphaMask
		BD_AlphaMask.RenderTarget[0].BlendEnable = FALSE;
		//-------------------------------------------------------// AlphaMaskAtoC
		BD_AlphaMaskAtoC.AlphaToCoverageEnable = TRUE;
 		BD_AlphaMaskAtoC.RenderTarget[0].BlendEnable = FALSE;
		//-------------------------------------------------------// Transperent
		BD_Transperent.RenderTarget[0].BlendEnable			 = TRUE;
		BD_Transperent.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
        BD_Transperent.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
        BD_Transperent.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
        BD_Transperent.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
        BD_Transperent.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
        BD_Transperent.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        BD_Transperent.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        RESULT OK;
		
		OK << v_device.GetDevice()->pDevice->CreateBlendState(&BD_Opaque,		&_blenders[(size_t)BlendMode::Opaque].GetBlender()->pState);
        OK << v_device.GetDevice()->pDevice->CreateBlendState(&BD_AlphaMask,		&_blenders[(size_t)BlendMode::AlphaMasked].GetBlender()->pState);
        OK << v_device.GetDevice()->pDevice->CreateBlendState(&BD_AlphaMaskAtoC,	&_blenders[(size_t)BlendMode::AlphaMaskedAtoC].GetBlender()->pState);
		OK << v_device.GetDevice()->pDevice->CreateBlendState(&BD_Transperent,	&_blenders[(size_t)BlendMode::Translucent].GetBlender()->pState);

        return OK.IsSucceed();
    }

    // virtual bool gpu_api_is_device_lost() = 0;
    // virtual bool gpu_api_device_update () = 0;

    //--------------------------------------------------------------------------// render state

    IMPL_CALL void gpu_api_bind_zbuffer_state(bool enabled)
    {
        assert(false);
    }

    IMPL_CALL void gpu_api_bind_topology   (impl_topology   *arg) { v_device.GetDevice()->pContext->IASetPrimitiveTopology(*arg->pState);            }
    IMPL_CALL void gpu_api_bind_sampler    (impl_sampler    *arg) { v_device.GetDevice()->pContext->PSSetSamplers(0,1,(&arg->pState));               }
    IMPL_CALL void gpu_api_bind_rasterizer (impl_rasterizer *arg) { v_device.GetDevice()->pContext->RSSetState(arg->pState);                         }
    IMPL_CALL void gpu_api_bind_blender    (impl_color_blender    *arg) { v_device.GetDevice()->pContext->OMSetBlendState(arg->pState, NULL, 0xffffffff);  }

    //--------------------------------------------------------------------------// gpu_buffers

    // virtual bool gpu_api_generate_vbuffer(void* data, size_t data_size, bool cpu_acsess) = 0;   //  Create vertex   buffer ind map it to gpu memory
    // virtual bool gpu_api_generate_ibuffer(void* data, size_t data_size, bool cpu_acsess) = 0;   //  Create index    buffer ind map it to gpu memory
    // virtual bool gpu_api_generate_cbuffer(void* data, size_t data_size, bool cpu_acsess) = 0;   //  Create constant buffer ind map it to gpu memory
    // 
    // virtual void gpu_api_set_vertex_buffer (api_impl_buffer*) = 0;
    // virtual void gpu_api_set_index_buffer  (api_impl_buffer*) = 0;
    // virtual void gpu_api_set_const_buffer  (api_impl_buffer*) = 0;
    // 
    // //--------------------------------------------------------------------------//  gpu_texture
    // 
    // virtual bool gpu_api_texture_from_file  (const std::string& utf8_path) = 0;
    // 
    // //--------------------------------------------------------------------------//  gpu_shader
    // 
    // virtual bool gpu_api_compilite_vertex_shader(const std::string& asci_code, const api_impl_shader_initdata& sid) = 0;
    // virtual bool gpu_api_compilite_pixel_shader (const std::string& asci_code, const api_impl_shader_initdata& sid) = 0;

};


using used_gpu_fubric = gpu_fubric_api_dx11;




