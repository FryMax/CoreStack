#pragma once

#include "../shared_in.h"

#if (GPU_API == __gpu_api_dx_11)
	#include "3d.api.dx11.h"
#endif 

#if (GPU_API == __gpu_api_opengl_2) || (GPU_API == __gpu_api_opengl_3)
	#include "gl.api.h"
#endif 

namespace gpu_api
{

#if (GPU_API == __gpu_api_dx_11)
	using used_gpu_fubric = gpu_fubric_api_dx11;
#endif 

#if (GPU_API == __gpu_api_opengl_2) || (GPU_API == __gpu_api_opengl_3)
	using used_gpu_factory = gpu_factory_opengl;
#endif 

	struct gpu_drawer : used_gpu_factory
	{
	public:
		virtual ~gpu_drawer() {}

	protected:
		struct FrameStatistic
		{
			size_t FrameN;
			double FrameTime;

			size_t DrawCalls;
			size_t TriangelsDrawed;
		};

		FrameStatistic LastFrame;

	public:
		virtual bool Init   (const RenderOptions&) = 0;
		virtual void Release() = 0;
 		virtual void Paint  () = 0;

		//--------------------
		//	Metric
		//--------------------
		
		const FrameStatistic GetLastFrameMetric()
		{
			return LastFrame;
		}

		//--------------------
		//	GPU ADAPTER
		//--------------------

		bool PresentRender() 
		{
			return gpu_api_present_render(); 
		}

 		void SetRenderTargets(impl_render_target* target, impl_depth_stencil* depth)
		{
			gpu_api_bind_render_targets(target, depth);	
		}
   		
		void ClearRenderTarget(impl_render_target* target, FLOAT4 color = FLOAT4(0, 0, 0, 0))
		{
			gpu_api_clear_render_target(color, target);   
		}
		
		void ClearDepthStencil(impl_depth_stencil* target, DepthStencilClearMode mode = DepthStencilClearMode::ClearFull)
		{
			gpu_api_clear_depth_stencil(mode, target);
		}
	
		//--------------------
		// Resize 
		//--------------------

		void SetFullScreenState (bool FullScr) { gpu_api_change_fullscreen_state(FullScr); }
		void GetFullScreenState ()             { gpu_api_get_fullscreen_state();        }
	
		void ResizeOutput(INT2 RenderSize, INT2 RealSize = {0,0})
		{
			if (RealSize == INT2{ 0,0 })
				RealSize = RenderSize;
		 
			if(gpu_api_resize_render(RenderSize.x, RenderSize.y, RealSize.x, RealSize.y))
			{
 				mRS.RenderCanvasW  = RenderSize.x;
				mRS.RenderCanvasH = RenderSize.y;
				mRS.RealCanvasW   = RealSize.x;
				mRS.RealCanvasH    = RealSize.y;
			}
		}
	
		//--------------------
		// DRAW CALLS 
		//--------------------

		void Draw                 (size_t count) { gpu_api_draw(count);		    }
		void DrawIndexed          (size_t count) { gpu_api_draw_indexed(count);	}
		void DrawIndexedInstansed (size_t count) { GetImpl()->uid(); }
		
		//--------------------
		// INIT 
		//--------------------

		bool InitDevice     () { return gpu_api_init_device      (); }
 		bool InitTopologys  () { return gpu_api_init_topologys   (); }
		bool InitSamplers   () { return gpu_api_init_samplers    (); }
		bool InitRasterizers() { return gpu_api_init_rasterizers (); }
		bool InitBlenders   () { return gpu_api_init_blenders    (); }

		//--------------------
		// RENDERER STATE 
		//--------------------

		void TurnZBufferOn  () { gpu_api_change_zbuffer_state (true);  }
		void TurnZBufferOff () { gpu_api_change_zbuffer_state (false); }

		void BindTopology   (const TopologyMode       mode){} //{ gpu_api_bind_topology  (_topologys  [(size_t)mode].GetTopology  ()); }
		void BindSampler    (const TextureSamplerMode mode){} //{ gpu_api_bind_sampler   (_samplers   [(size_t)mode].GetSampler   ()); }
		void BindRasterizer (const RasteriserMode     mode){} //{ gpu_api_bind_rasterizer(_rasterizers[(size_t)mode].GetRasterizer()); }
		void BindBlender    (const ColorBlendMode     mode){} //{ gpu_api_bind_blender   (_blenders   [(size_t)mode].GetBlender   ()); }

		//--------------------
		// SHADERS
		//--------------------

	};
}