#pragma once

#include "../shared_in.h"

#include "gpu.drawer.h"

namespace addon_gpu_api
{
	using namespace gpu_api;

	class NonAssignable {
	public:
		NonAssignable(NonAssignable const&) = delete;
		NonAssignable& operator=(NonAssignable const&) = delete;
		NonAssignable() {}
	};

	struct GpuTexture : gpu_texture, NonAssignable
	{
	public:
		GpuTexture() = default;

		~GpuTexture()
		{
			gpu_texture::Release();
		}
		
	public:
		void CreateUpdateData(const bitmap32& bitmap, const texture_state* opt = nullptr)
		{
			if (get_size() != bitmap.size())
			{
				texture_state state;

				if (opt)
					std::memcpy(&state, opt, sizeof(texture_state));

				state.w = bitmap.size().x;
				state.h = bitmap.size().y;
				//state.mip_levels = 0;

				gpu_texture::Release();
				gpu_texture::CreateTex(state, bitmap.raw_data());
			}

			gpu_texture::UpdateData(bitmap.raw_data());
		}

		void BindSampler(gpu_sampler& sampler)
		{
			gpu_texture::api_bind(0);
			sampler.Bind();
			gpu_texture::api_unbind();
		}

	protected:
	};


	void print_gl_caps()
	{
		int max;
		log::debug << str::format::insert("GL device caps:");

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);			log::debug << str::format::insert("GL_MAX_TEXTURE_SIZE: {}", max);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max);	log::debug << str::format::insert("GL_MAX_TEXTURE_IMAGE_UNITS: {}", max);
	}


};

using namespace addon_gpu_api;