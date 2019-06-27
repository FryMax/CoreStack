#pragma once

// enc

#include "graphics.h"

#include "libde265/en265.h"
#include "libde265/configparam.h"
#include "libde265/image-io.h"
#include "libde265/encoder/encoder-core.h"
#include "libde265/util.h"

namespace
{
	struct inout_params_
	{
		inline inout_params_()
		{
			input_yuv.set_ID("input"); input_yuv.set_short_option('i');
			input_yuv.set_default("paris_cif.yuv");

			output_filename.set_ID("output"); output_filename.set_short_option('o');
			output_filename.set_default("out.bin");

			reconstruction_yuv.set_ID("input");
			reconstruction_yuv.set_default("recon.yuv");

			first_frame.set_ID("first-frame");
			first_frame.set_default(0);
			first_frame.set_minimum(0);

			max_number_of_frames.set_ID("frames");
			max_number_of_frames.set_short_option('f');
			max_number_of_frames.set_minimum(1);
			//max_number_of_frames.set_default(INT_MAX);

			input_width.set_ID("width"); input_width.set_short_option('w');
			input_width.set_minimum(1);  input_width.set_default(352);

			input_height.set_ID("height"); input_height.set_short_option('h');
			input_height.set_minimum(1); input_height.set_default(288);

			input_is_rgb.set_ID("rgb");
			input_is_rgb.set_default(false);
			input_is_rgb.set_description("input is sequence of RGB PNG images");
		}

		inline void register_params(config_parameters& config)
		{
			config.add_option(&input_yuv);
			config.add_option(&output_filename);
			config.add_option(&first_frame);
			config.add_option(&max_number_of_frames);
			config.add_option(&input_width);
			config.add_option(&input_height);
		}
		
		// input

		option_int first_frame;
		option_int max_number_of_frames;

		option_string input_yuv;
		option_int input_width;
		option_int input_height;

		option_bool input_is_rgb;

		// output

		option_string output_filename;

		// debug

		option_string reconstruction_yuv;


 	};


 
}

 


namespace addon_lib265
{
	struct e265frame
	{
		void release()
		{
			delete[] data;
		}

		uint8_t* data;
		size_t size;
	};

	class Encoder265
	{
	public:
		inline bool init(int w, int h)
		{
			de265_init();
			mEctx = en265_new_encoder();

 			//ImageSource_YUV *image_source_yuv = new ImageSource_YUV;
			//mImageSource = image_source_yuv;
			
 			en265_show_parameters(mEctx);
			en265_start_encoder(mEctx, 0);

			return true;
		}

		inline void release() 
		{
			// --- print statistics ---
			en265_print_logging((encoder_context*)mEctx, "tb-split", nullptr);
			en265_free_encoder(mEctx);
			de265_free();
		}

		// Note: push nullptr to indicate stream end
		//
		inline void push_image_yuv420(const uint8_t* yuv420[3], int w, int h)
		{
			de265_image* input_image = nullptr;

			if (yuv420)
			{
				const uint8_t* pY  = yuv420[0];
				const uint8_t* pCr = yuv420[1];
				const uint8_t* pCb = yuv420[2];
				const int width = w;
				const int height = h;

				de265_image* img = new de265_image;
				img->alloc_image(width, height, de265_chroma_422, NULL, false, NULL, /*NULL,*/ 0, NULL, false);
				assert(img); // TODO: error handling

				// --- load image ---

				uint8_t* chanelA = img->get_image_plane(0);
				uint8_t* chanelB = img->get_image_plane(1);
				uint8_t* chanelC = img->get_image_plane(2);
				int strideA = img->get_image_stride(0);
				int strideB = img->get_image_stride(1);
				int strideC = img->get_image_stride(2);

				for (int y = 0; y < height; y++) std::memcpy(chanelA + y * strideA, pY,  width);
				for (int y = 0; y < height; y++) std::memcpy(chanelB + y * strideB, pCr, width / 2);
				for (int y = 0; y < height; y++) std::memcpy(chanelC + y * strideC, pCb, width / 2);

				input_image = img;
			}

			if (input_image == nullptr) 
				en265_push_eof(mEctx);
			else 
				en265_push_image(mEctx, input_image);
		}

		inline void start_encode()
		{
			// encode images while more are available
			en265_encode(mEctx);
		}
		
		inline e265frame get_encoded_frame(bool wait = false)
		{
			e265frame out{ nullptr, 0 };

			size_t wait_timeout_ms = wait ? -1 : 0;
			en265_packet* pck = en265_get_packet(mEctx, wait_timeout_ms);

			if (pck != nullptr)
			{
				out.size = pck->length;
				out.data = new uint8_t[pck->length];
				std::memcpy(out.data, pck->data, pck->length);
				en265_free_packet(mEctx, pck);
			}

			return out;
		}

	protected:

		en265_encoder_context* mEctx = nullptr;
		//ImageSource* mImageSource;
		inout_params_ inout_params;

	};
	
 
}



