#pragma once

#if false // not used


#include "globals.h"
#include <zlib.h>

DEF_UMODULE(zlib_module, "zlib lib", "1.2.11",
"Site:   https://zlib.net""\n"
"Github: https://github.com/madler/zlib"
);

namespace libs
{
 	namespace zlib
	{
 	}
}

LIB_NAMESPACE_START

namespace str
{
	namespace compress
	{
		enum class CompessType
		{
 			DefaultCompress,
			NoCompress,
			BestCompress,
			BestSpeed,

			Default = DefaultCompress,
		};

		//	throw std::runtime_error
		//	todo fit in runtime stack(16384)
		inline std::string compress_string	(const std::string& str, CompessType ct = CompessType::Default)
		{
			int	compressionlevel = Z_DEFAULT_COMPRESSION;

			switch (ct)
			{
			case CompessType::DefaultCompress:	compressionlevel = Z_DEFAULT_COMPRESSION;	break;
			case CompessType::NoCompress:		compressionlevel = Z_NO_COMPRESSION;		break;
			case CompessType::BestCompress:		compressionlevel = Z_BEST_COMPRESSION;		break;
			case CompessType::BestSpeed:		compressionlevel = Z_BEST_SPEED;			break;
			default:							compressionlevel = Z_DEFAULT_COMPRESSION;	break;
			}

			z_stream zs;                        // z_stream is zlib's control structure
			memset(&zs, 0, sizeof(zs));

			if (deflateInit(&zs, compressionlevel) != Z_OK)
				throw(std::runtime_error("deflateInit failed while compressing."));

			zs.next_in = (Bytef*)str.data();
			zs.avail_in = str.size();           // set the z_stream's input

			int ret;
			char outbuffer[32768];
			std::string outstring;

			// retrieve the compressed bytes blockwise
			do {
				zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
				zs.avail_out = sizeof(outbuffer);

				ret = deflate(&zs, Z_FINISH);

				if (outstring.size() < zs.total_out) {
					// append the block to the output string
					outstring.append(outbuffer,
						zs.total_out - outstring.size());
				}
			} while (ret == Z_OK);

			deflateEnd(&zs);

			if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
				std::ostringstream oss;
				oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
				throw(std::runtime_error(oss.str()));
			}

			return outstring;
		}

		inline std::string decompress_string(const std::string& str)
		{
			z_stream zs;                        // z_stream is zlib's control structure
			memset(&zs, 0, sizeof(zs));

			if (inflateInit(&zs) != Z_OK)
				throw(std::runtime_error("inflateInit failed while decompressing."));

			zs.next_in = (Bytef*)str.data();
			zs.avail_in = str.size();

			int ret;
			char outbuffer[32768];
			std::string outstring;

			// get the decompressed bytes blockwise using repeated calls to inflate
			do {
				zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
				zs.avail_out = sizeof(outbuffer);

				ret = inflate(&zs, 0);

				if (outstring.size() < zs.total_out) {
					outstring.append(outbuffer,
						zs.total_out - outstring.size());
				}

			} while (ret == Z_OK);

			inflateEnd(&zs);

			if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
				std::ostringstream oss;
				oss << "Exception during zlib decompression: (" << ret << ") "
					<< zs.msg;
				throw(std::runtime_error(oss.str()));
			}

			return outstring;
		}

		struct z_buffer
		{
			inline void release()
			{
				std::free(data);
			}

			void* data = nullptr;
			size_t comp_size = 0;
			size_t orig_size = 0;
		};

		inline z_buffer compress_data(const void* data, const size_t size, CompessType ct = CompessType::Default)
		{
			z_buffer out;

			out.orig_size = size;
			out.data = (uint8_t*)std::malloc(size);

			if (!out.data)
				throw std::bad_alloc();

			//////////////////////////////////////////////////////////////////////////
			int	cmp_level = 0;

			switch (ct)
			{
			case CompessType::DefaultCompress:	cmp_level = Z_DEFAULT_COMPRESSION;	break;
			case CompessType::NoCompress:		cmp_level = Z_NO_COMPRESSION;		break;
			case CompessType::BestCompress:		cmp_level = Z_BEST_COMPRESSION;		break;
			case CompessType::BestSpeed:		cmp_level = Z_BEST_SPEED;			break;
			default:							cmp_level = Z_DEFAULT_COMPRESSION;	break;
			}

			z_stream zb;
			std::memset(&zb, 0, sizeof(zb));

			if (deflateInit(&zb, cmp_level) != Z_OK)
				throw std::runtime_error("deflateInit failed while compressing");

			zb.next_in   = (Bytef*)data;
			zb.next_out  = reinterpret_cast<Bytef*>(out.data);
			zb.avail_in  = size;
			zb.avail_out = size;

			int result = deflate(&zb, Z_FINISH);
			deflateEnd(&zb);

			if (result != Z_STREAM_END)
				throw std::runtime_error(std::string(zb.msg));

			out.comp_size = zb.total_out;
			out.data = (uint8_t*)std::realloc(out.data, out.comp_size);

			if (!out.data)
				throw std::runtime_error("realloc failed");

			return out;
		}

		inline bool decompress_data(z_buffer compressed, void *dest)
		{
			if (!compressed.data)
				throw std::runtime_error("input buffer is missing");

			if(!dest)
				throw std::runtime_error("output buffer is missing");

			z_stream zb;
			std::memset(&zb, 0, sizeof(zb));

			if (inflateInit(&zb) != Z_OK)
				throw(std::runtime_error("inflateInit failed while decompressing."));

			zb.next_in  = reinterpret_cast<Bytef*>(compressed.data);
			zb.next_out = reinterpret_cast<Bytef*>(dest);
			zb.avail_in = compressed.comp_size;
			zb.avail_out = compressed.orig_size;

			int ret = inflate(&zb, 0);
			inflateEnd(&zb);

			if (ret != Z_STREAM_END)
				throw std::runtime_error(std::string(zb.msg));
		}


	}

}

LIB_NAMESPACE_END


#endif