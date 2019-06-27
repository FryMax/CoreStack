#pragma once

#include "globals.h"
#include "math.types.h"
#include "math.algo.h"
#include "art.color.h"
#include "utils.digits.h"

#define SIMD_ALIGNMENT_SSE          (16 * 8)
#define SIMD_ALIGNMENT_AVX          (32 * 8)
#define SIMD_ALIGNMENT_AVX512       (64 * 8)
#define SIMD_ALIGNMENT_NEON         (16 * 8) // ARM

#define GR_SIMD_ALIGNMENT_TARGET    (SIMD_ALIGNMENT_SSE)
#define GR_ALIGNED_PLACEMENT        false//(true)
#define GR_COLOR_ALIGNMENT          (GR_ALIGNED_PLACEMENT ? 4 : 1) // not used, mb dell
#define GR_BITMAP_ALIGNMENT         (GR_ALIGNED_PLACEMENT ? GR_SIMD_ALIGNMENT_TARGET : 1)
#define GR_BITMAP_REALLOCK_ALLOWED  (false)

LIB_NAMESPACE_START

namespace art
{
	using namespace art::color;

	enum class scalefilter : unsigned
	{
        no_scale,  // result not specified
		nearest,   //
		bilinear,  //
		mitchell_netravali,

		smart, // smart?

		//mipmap_bilinear,
		//trilinear,
		//mipmap_trilinear,

		ENUM_SIZE,
	};

	namespace
    {
        void scaleNearestNeighbor(
            byte* out, unsigned outW, unsigned outH,
			byte* src, unsigned srcW, unsigned srcH, unsigned chanCount)
        {
            FUSE_IF(!out, "Output ptr is null", return);
            FUSE_IF(!src, "Source ptr is null", return);
            FUSE_IF(!chanCount, "Channels count can't be zero", return);

            for (unsigned iy = 0; iy < outH; iy++) {
            for (unsigned ix = 0; ix < outW; ix++) {

                const unsigned sX = static_cast<unsigned>((ix + 0.5f) * srcW) / outW;
                const unsigned sY = static_cast<unsigned>((iy + 0.5f) * srcH) / outH;
                const unsigned outOffset = iy * outW + ix;
                const unsigned srcOffset = sY * srcW + sX;

                std::memcpy(&out[outOffset], &src[srcOffset], sizeof(byte) * chanCount);
            }
            }
        }

		void scaleBilinear(
			byte* out, unsigned outW, unsigned outH,
			byte* src, unsigned srcW, unsigned srcH, unsigned chanCount)
		{

            //stbir__resize_arbitrary(NULL,
			//	src, srcW, srcH, srcW * sizeof(byte) * chanCount,
			//	out, outW, outH, outW * sizeof(byte) * chanCount,
            //    0, 0, 1, 1, NULL, chanCount, -1, 0,
            //    STBIR_TYPE_UINT8, STBIR_FILTER_BOX, STBIR_FILTER_BOX,
			//	STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
			//	STBIR_COLORSPACE_LINEAR);
		}

        void scaleMitchellNetravali(
            byte* out, unsigned outW, unsigned outH,
            byte* src, unsigned srcW, unsigned srcH, unsigned chanCount);

        void scaleSmart(
            byte* out, unsigned outW, unsigned outH,
            byte* src, unsigned srcW, unsigned srcH, unsigned chanCount);

        constexpr float LERP(float a, float b, float t)
        {
            return (1.0f - t) * a + t * b;
        }
	}


	static_assert(GR_BITMAP_ALIGNMENT > 0,     "bitmap_t<T> : invalid alignment value");
	static_assert(!GR_BITMAP_REALLOCK_ALLOWED, "bitmap_t<T> : realloc is not provided");

	template<class T>
	struct bitmap_t
	{
	public:
		const size_t psize           = sizeof(T);
		const size_t alignment       = GR_BITMAP_ALIGNMENT;
		const bool   is_aligned      = GR_ALIGNED_PLACEMENT;
		const bool   realloc_allowed = GR_BITMAP_REALLOCK_ALLOWED;

		//const std::unique_ptr<size_t> view_count = new size_t;
	protected:
		size_t w = 0;
		size_t h = 0;

		// TODO mb change to std::byte*
		T* data = nullptr;

		void _mem_resize(size_t size)
		{
			BREAK_IF(size == 0);

			_mem_free();

			if (is_aligned)
			{
				data = reinterpret_cast<T*>(_allocate_aligned(data, size, alignment));
				BREAK_IF(reinterpret_cast<uintptr_t>(data) % alignment != 0)
			}
			else
			{
				data = reinterpret_cast<T*>(std::malloc(size));
			}

			if (!data)
				throw std::bad_alloc();
		}

		void _mem_free()
		{
			if (!data)
				return;

			is_aligned ? _free_aligned(data) : std::free(data);

			data = nullptr;
		}

		void _resize(size_t W, size_t H)
		{
			w = W;
			h = H;

			if (w != 0 && h != 0)
				_mem_resize(w * h * psize);
			else
				_mem_free();
		}

	public:
		virtual inline ~bitmap_t()
		{
			_mem_free();
		};

		constexpr bitmap_t() noexcept
		{

		};

		constexpr bitmap_t(CINT2 size)
		{
			_resize(size.x, size.y);
		}

		constexpr bitmap_t(CINT2 size, const T& color)
		{
			_resize(size.x, size.y);
			fill_by_color(color);
		}

		constexpr bitmap_t(size_t _w, size_t _h) //mbdell
		{
			_resize(_w, _h);
		}

		constexpr bitmap_t(size_t _w, size_t _h, const T& color) //mbdell
		{
			_resize(_w, _h);
			fill_by_color(color);
		}
		//------------------------------------------------------//  MOVE_CONSTRUCT
		constexpr bitmap_t(bitmap_t&& obj) noexcept
		{
			w = obj.w;
			h = obj.h;
			data = obj.data;

			obj.w = 0;
			obj.h = 0;
			obj.data = nullptr;
		}
		//------------------------------------------------------//  MOVE_ASSIGN [free]
		constexpr void operator=(bitmap_t&& obj) noexcept
		{
			_mem_free();

			w = obj.w;
			h = obj.h;
			data = obj.data;

			obj.w = 0;
			obj.h = 0;
			obj.data = nullptr;
		}
		//------------------------------------------------------//  COPY_CONSTR [alloc] // TODO mb change to = deleted; allow only explicit .get_copy()
		constexpr bitmap_t(const bitmap_t& inst)
		{
			BREAK_IF(data == inst.data);

			_resize(inst.w, inst.h);
			std::memcpy(data, inst.data, w * h * psize);
		}

		constexpr void operator=(const bitmap_t& inst)
		{
			BREAK_IF(data == inst.data);

			_resize(inst.w, inst.h);
			std::memcpy(data, inst.data, w * h * psize);
		}

		constexpr void operator=(bitmap_t* ptr) = delete;
		constexpr void operator=(const bitmap_t* ptr) = delete;

	public:
		constexpr bool operator==(const bitmap_t& inst)
		{
			if (w != inst.w || h != inst.h)
				return false;

			// TODO replace by something that stops on first diff
			return std::memcmp(data, inst.data, w * h * psize) == 0;
		}

		//------------------------------------------------------//	set

		inline void fill_by_color(const T& color) noexcept
		{
			std::fill(data, data + w * h, color);
		}

		inline void copy_from_ptr(const void* ptr, CINT2 size)
		{
			BREAK_IF(!ptr);

			_resize(size.x, size.y);
			std::memcpy(data, ptr, w * h * psize);
		}

		inline bitmap_t get_copy_scaled(CINT2 size, scalefilter sc_filter = scalefilter::nearest) const
		{
			bitmap_t buff(size.x, size.y);

			write_scalled_to_ptr(buff.raw_data(), size, sc_filter);

			return std::move(buff);
		}

		inline void resize(CINT2 size, scalefilter sc_filter = scalefilter::no_scale)
		{
			bitmap_t buff(size.x, size.y);

			write_scalled_to_ptr(buff.raw_data(), size, sc_filter);

			*this = std::move(buff);
		}

		inline void write_scalled_to_ptr(const void* out, CINT2 out_size, scalefilter sc_filter = scalefilter::no_scale) const
		{
			FUSE_IF(!out, "Scale output buffer is nullptr", return);

			const int channels = sizeof(T) / sizeof(uint8_t);

			switch (sc_filter)
			{
			default:

			break; case scalefilter::no_scale:

				// thats ok

			break; case scalefilter::nearest:

				scaleNearestNeighbor(
					(uint8_t*)out,
                    (unsigned)out_size.x,
                    (unsigned)out_size.y,
					(uint8_t*)this->data,
					(unsigned)this->w,
					(unsigned)this->h,
					(unsigned)channels);

			break; case scalefilter::bilinear:

				scaleBilinear(
                    (uint8_t*)out,
                    (unsigned)out_size.x,
                    (unsigned)out_size.y,
                    (uint8_t*)this->data,
					(unsigned)this->w,
					(unsigned)this->h,
					(unsigned)channels);

				//break; case scalefilter::stb_bilinear:

                //stbir__resize_arbitrary(NULL,
                //	(uint8_t*) this->data,         // src
                //	(size_t)   this->size().x,     //
                //	(size_t)   this->size().y,     //
                //	(size_t)   this->size().x * 4, //
                //	(uint8_t*) out,                // out
                //	(size_t)   out_size.x,         //
                //	(size_t)   out_size.y,         //
                //	(size_t)   out_size.x * 4,     //
                //	0, 0, 1, 1, NULL, 4, -1, 0,
                //	STBIR_TYPE_UINT8, STBIR_FILTER_BOX, STBIR_FILTER_BOX, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

			//break; case scalefilter::stb_mitchell_netravali:

				//stbir__resize_arbitrary(NULL,
				//	(uint8_t*) this->data,         // src
				//	(size_t)   this->size().x,     //
				//	(size_t)   this->size().y,     //
				//	(size_t)   this->size().x * 4, //
				//	(uint8_t*) out,                // out
				//	(size_t)   out_size.x,         //
				//	(size_t)   out_size.y,         //
				//	(size_t)out_size.x * 4,        //
				//	0, 0, 1, 1, NULL, 4, -1, 0,
				//	STBIR_TYPE_UINT8,
				//	STBIR_FILTER_MITCHELL, STBIR_FILTER_MITCHELL, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

			//break; case scalefilter::stb_auto:

			//	stbir_resize_uint8(
			//		(uint8_t*)this->data,       // src
			//		(size_t)this->size().x,     //
			//		(size_t)this->size().y,     //
			//		(size_t)this->size().x * 4, //
			//		(uint8_t*)out,              // out
			//		(size_t)out_size.x,         //
			//		(size_t)out_size.y,         //
			//		(size_t)out_size.x * 4,     //
			//		(size_t)4);
			//
			//	break;
			}
		}

		// mb dell

		inline void for_each_in_region(INT2 pos, INT2 size, std::function<void(T&, const INT2& pos)> for_each_func, int _thread_count = -1)
		{
			LOG_WARN_NO_IMPL;
		}

		inline void for_each(std::function<void(T&, const INT2& pos)> for_each_call, int _thread_count = -1)
		{
#if !EM_BUILD
			assert(_thread_count != 0);
			static size_t thread_c = std::thread::hardware_concurrency();

			if (_thread_count != -1)
				thread_c = _thread_count;
			else
			{
				if (thread_c > 4) thread_c = 4;
				//if (thread_c < 4) thread_c = 2;
				//if (thread_c < 2) thread_c = 1;
			}
			if (thread_c == 1)
			{
				ForArray(x, w)
				ForArray(y, h)
					for_each_call(data[y * w + x], INT2(x, y));
				return;
			}

			//-----------------------------------------------------------//
			struct compute_range
			{
				size_t y_start;
				size_t y_size;
			};

			std::vector<compute_range> ranges(thread_c);

			int range = h / thread_c;
			int last_range = h - (range * (thread_c - 1));

			ForArray(i, thread_c - 1)
			{
				ranges[i].y_start = i * range;
				ranges[i].y_size = range;
			}
			ranges[thread_c - 1].y_start = (thread_c - 1) * range;
			ranges[thread_c - 1].y_size = last_range;

			//ForArray(i, ranges.size())
			//	log::info << std::to_string(ranges[i].y_start) + " - " + std::to_string(ranges[i].y_size);

			//-----------------------------------------------------------//
			std::vector<std::thread> th_pool(thread_c);

			//ForArray(i, thread_c)
				//log::info << std::to_string(thread_c);

			ForArray(i, thread_c)
			{
				int st = ranges[i].y_start;
				int sz = ranges[i].y_start + ranges[i].y_size;
				int wi = w;
				T* dat = data;
				auto func = for_each_call;

				th_pool[i] = std::thread([st, sz, wi, dat, func]()
					{
						int start = st;
						int end = sz;

						ForArrayFrom(y, start, end)
							ForArray(x, wi)
								func(dat[y * wi + x], INT2(x, y));
					}
				);
			}

			ForArray(i, thread_c)
				//if (th_pool[i].joinable())
					th_pool[i].join();

#endif // EM_BUILD
		}

		inline void sub_region(std::function<void(const INT2& start, const INT2& end)> func, int _region_count = 0)
		{
#if !EM_BUILD
			const size_t concurrency = std::thread::hardware_concurrency();

			size_t thread_count = 0;

			if (_region_count > 0)
			{
				thread_count = _region_count;
			}
			else
			{
				if (concurrency > 4) thread_count = 4;
				//if (concurrency < 4) thread_count = 2;
				//if (concurrency < 2) thread_count = 1;
			}

			if (thread_count == 1)
			{
				func({ 0,0 }, { (int)w,(int)h });

				return;
			}

			//-----------------------------------------------------------//
			struct compute_range
			{
				size_t y_start;
				size_t y_size;
			};

			std::vector<compute_range> ranges(thread_count);

			int range = h / thread_count;
			int last_range = h - (range * (thread_count - 1));

			ForArray(i, thread_count - 1)
			{
				ranges[i].y_start = i * range;
				ranges[i].y_size = range;
			}
			ranges[thread_count - 1].y_start = (thread_count - 1) * range;
			ranges[thread_count - 1].y_size = last_range;

			//ForArray(i, ranges.size())
			//	log::info << std::to_string(ranges[i].y_start) + " - " + std::to_string(ranges[i].y_size);

			//-----------------------------------------------------------//
			std::vector<std::thread> th_stack(thread_count);

			//ForArray(i, thread_c)
				//log::info << std::to_string(thread_c);

			ForArray(i, thread_count)
			{
				int i_y_beg = ranges[i].y_start;
				int i_y_end = ranges[i].y_start + ranges[i].y_size;
				int i_w = w;
				//T* dat = data;
				//auto func = for_each_func;

				th_stack[i] = std::thread([i_y_beg, i_y_end, i_w, func]()
					{
						INT2 start = { 0, i_y_beg };
						INT2 end = { i_w, i_y_end };

						func(start, end);
					}
				);
			}

			ForArray(i, thread_count)
				//if (th_stack[i].joinable())
				th_stack[i].join();
#endif // EM_BUILD
		}


		//------------------------------------------------------//	get
		constexpr INT2 size() const noexcept
		{
			return INT2(w, h);
		}

		constexpr T* begin() const noexcept
        {
            return data;
        }

        constexpr T* end() const noexcept
        {
            return data + pixel_count();
        }

		inline size_t pixel_count() const noexcept
		{
			return w * h;
		}

		inline size_t pixel_size() const noexcept
		{
			return psize;
		}

		inline size_t data_size() const noexcept
		{
			return w * h * psize;
		}

		//////////////////////////////////////////////////////////////////////////

        constexpr const T* const raw_data() const
        {
            return data;
        }

		constexpr const T* const raw_data(int x, int y) const
        {
            BREAK_IF(!is_point_in({ x, y }));
            return &data[y * w + x];
        }

		inline T* raw_data(int x = 0)
		{
			BREAK_IF(x < 0 || x >= w * h);
			return &data[x];
		}

		inline T* raw_data(int x, int y)
		{
			BREAK_IF(!is_point_in({x, y}));
			return &data[y * w + x];
		}

		inline T* raw_data(CINT2 p)
		{
			BREAK_IF(!is_point_in(p));
			return &data[p.y * w + p.x];
		}

		// move to bitmap_view or use non-virtual static template impl
		// virtual inline T* view_data(int x = 0, int y = 0) const
		// {
		//     return raw_data(x, y);
		// }

		inline T get_pixel(int x, int y) const
		{
			return data[y * w + x];
		}

		inline bitmap_t get_copy() const
		{
			return bitmap_t(*this);
		}

		inline bitmap_t get_subregion_copy(INT2 pos, INT2 size) const
		{
			//todo miror/warp/back_color if region outs of img bounds
			BREAK_IF(!is_rect_in(pos, size));

			bitmap_t out(size.x, size.y);

			ForArray(y, size.y)
				std::memcpy(out.raw_data(0, y), raw_data(pos.x, pos.y + y), size.x * psize);

			return out;
		}

	public:
        constexpr bool is_point_in(CINT2 pos) const noexcept
        {
            return 0 <= pos.x && pos.x < w
                && 0 <= pos.y && pos.y < h;
        }

        constexpr bool is_region_in(CINT2 a, CINT2 b) const noexcept
        {
            return is_point_in(a)
				&& is_point_in(b);
        }

		constexpr bool is_rect_in(CINT2 pos, CINT2 size) const noexcept
		{
			return is_point_in(pos)
				&& is_point_in(pos + size - INT2{1,1});

            //BREAK_IF(size.x < 0);
            //BREAK_IF(size.y < 0);
            //return pos.x >= 0
            //	&& pos.y >= 0
            //	&& pos.x + size.x <= w
            //	&& pos.y + size.y <= h;
		}
	};

	using bitmap24 = bitmap_t<color::color24>;
	using bitmap32 = bitmap_t<color::color32>;

	template<class T>
	struct bitmap_view : bitmap_t<T>
	{
	protected:
		const size_t span;   // src bitmap line size
		const size_t view_x; // from left
		const size_t view_y; // from top
		const size_t view_w; // hors size
		const size_t view_h; // vert size
		T* view_ptr = nullptr;

	public:
		inline bitmap_view(const bitmap_t<T>& src_inst, INT2 pos = { -1,-1 }, INT2 size = { -1,-1 })
		{
			if (pos  == INT2{ -1, -1 }) pos  = { 0, 0 };
			if (size == INT2{ -1, -1 }) size = src_inst.size();

			//assert(is_in(pos, {0,0}));
			assert(src_inst.is_rect_in(pos, size));

			//w = src_inst.w;
			//h = src_inst.w;
			//data = src_inst.data;

			view_x = pos.x;
			view_y = pos.y;
			view_w = size.x;
			view_h = size.y;

			span = src_inst.size().x;
			view_ptr = src_inst.raw_data(pos.x, pos.y);
		}

		inline ~bitmap_view()
		{
			// disable parent destruction
			// mb change to
			//    parent->view_count--
			(this)->data = nullptr;
		}

		inline T* view_data(int x = 0, int y = 0) const
		{
			return &view_ptr[y * span + x];
		}

		constexpr INT2 size() const
		{
			return INT2(view_w, view_h);
		}

	};

	using bitmap24_view = bitmap_view<color::color24>;
	using bitmap32_view = bitmap_view<color::color32>;

	struct bitmap_converter
	{
		static uint8_t hex_to_short(const std::string& val);

		static std::string bitmap_to_string(const bitmap32& img);

		static bitmap32 string_to_bitmap(INT2 size, const std::string& str);
	};

	namespace draw
	{
		color32 sample(const FLOAT2& uv);

		constexpr const color32* sampleTex2D(const FLOAT2& uv, const bitmap32& img);

        constexpr color32 sample(const color4f& color)
        {
            return {
                static_cast<uint8_t>(color.x * 255.f),
                static_cast<uint8_t>(color.y * 255.f),
                static_cast<uint8_t>(color.z * 255.f),
                static_cast<uint8_t>(color.w * 255.f),
            };
        }

        constexpr color32 lerp(const color32& a, const color32& b, float t)
        {
            //todo mb opt
            return {
                static_cast<uint8_t>((1.0f - t) * a.r + t * b.r),
                static_cast<uint8_t>((1.0f - t) * a.g + t * b.g),
                static_cast<uint8_t>((1.0f - t) * a.b + t * b.b),
                static_cast<uint8_t>((1.0f - t) * a.a + t * b.a),
            };
        }

        constexpr FLOAT2 lerp(CFLOAT2 a, CFLOAT2 b, float t)
        {
            return {
                ((1.0f - t) * a.x + t * b.x),
                ((1.0f - t) * a.y + t * b.y),
            };
        }

        constexpr FLOAT3 lerp(CFLOAT3 a, CFLOAT3 b, float t)
        {
            return {
                ((1.0f - t) * a.x + t * b.x),
                ((1.0f - t) * a.y + t * b.y),
                ((1.0f - t) * a.z + t * b.z),
            };
        }

        constexpr FLOAT4 lerp(CFLOAT4 a, CFLOAT4 b, float t)
        {
            return {
                ((1.0f - t) * a.x + t * b.x),
                ((1.0f - t) * a.y + t * b.y),
                ((1.0f - t) * a.z + t * b.z),
                ((1.0f - t) * a.w + t * b.w),
            };
        }

        constexpr float lerp(float a, float b, float t)
        {
            return (1.0f - t) * a + t * b;
        }

        constexpr float edgeFunction2D(CFLOAT3 a, CFLOAT3 b, CFLOAT3 c)
        {
            return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
        }

        constexpr float edgeFunction2D(CFLOAT4 a, CFLOAT4 b, CFLOAT4 c)
        {
            return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
        }

		//////////////////////////////////////////////////////////////////////////

		static bitmap32 chess_board(INT2 size, unsigned block_size = 8)
		{
			BREAK_IF(block_size == 0);

			// TODO
			// move to colors
			constexpr color32 white		 = { 255, 255, 255, 255 };
			constexpr color32 light_gray = { 200, 200, 200, 255 };

			bitmap32 out(size.x, size.y, light_gray);

			for (int y = 0; y < size.y; y++)
			for (int x = 0; x < size.x; x++)
			{
				if ((x % block_size * 2 < block_size) ^
					(y % block_size * 2 < block_size))
				{
					*out.raw_data(x, y) = white;
				}
			}

			return out;
		}

		// Note:
		//  don't forget add {0.5, 0.5} to coord`s for good-looking line
		//
		inline void drawline2D(bitmap32& surfase,
			FLOAT2 posA,
			FLOAT2 posB,
			color32 colorA,
			color32 colorB)
		{
			const bool in_rect = surfase.is_point_in({posA.x, posA.y})
			                  && surfase.is_point_in({posB.x, posB.y });

			const int iter_count = 1 + static_cast<int>(std::max(
					std::fabs(posB.x - posA.x),
					std::fabs(posB.y - posA.y)));

			const float offx = (posB.x - posA.x) / iter_count;
			const float offy = (posB.y - posA.y) / iter_count;

			for (int i = 0; i <= iter_count; i++)
			{
				const float dim = float(i) / iter_count;

				const int x = static_cast<int>(lerp(posA.x, posB.x, dim));
				const int y = static_cast<int>(lerp(posA.y, posB.y, dim));

				const color32 col = lerp(colorA, colorB, dim);

				if (in_rect)
				{
					*surfase.raw_data(x, y) = col;
				}
				else
				{
					if (surfase.is_point_in({x, y}))
					{
						*surfase.raw_data(x, y) = col;
					}
				}
			}
		}

		struct trg_half
		{
			trg_half(FLOAT3& _Aa, FLOAT3& _Ab, FLOAT3& _Ba, FLOAT3& _Bb) :
				AA(_Aa),
				AB(_Ab),
				BA(_Ba),
				BB(_Bb),
				iter_count(static_cast<int>(_Ab.y - _Aa.y)) {}

			const FLOAT3& AA;
			const FLOAT3& AB;
			const FLOAT3& BA;
			const FLOAT3& BB;
			const int iter_count;
		};

		struct vertex
		{
			FLOAT4 pos;
			FLOAT4 col;
			FLOAT2 uv;
		};

        extern FLOAT4 gCorrection;

        //template<typename Type>
		//bitmap_t<float>* depthbuffer,
		void drawTriangle3D_color_tex(bitmap32& surface, const bitmap32& texture, vertex A, vertex B, vertex C);

		template<typename Type>
		inline void drawTriangle3D(
			bitmap32& surface,
			bitmap_t<float>* depthbuffer,
			FLOAT3 posA,
			FLOAT3 posB,
			FLOAT3 posC,
			Type atrA,
			Type atrB,
			Type atrC,
			bool wire = true)
		{
			///------- sort by y-coord

			if (posB.y < posA.y) { std::swap(posB, posA); std::swap(atrB, atrA); }
			if (posB.y > posC.y) { std::swap(posB, posC); std::swap(atrB, atrC); }
			if (posB.y < posA.y) { std::swap(posB, posA); std::swap(atrB, atrA); }

			///------- find D pos

			const float dlp = 1.0f - (posC.y - posB.y) / (posC.y - posA.y);
			FLOAT3 posD(lerp(posA.x, posC.x, dlp), posB.y, 0);

			//-- opt
			const float Aw = 1.0f / posA.z;
			const float Bw = 1.0f / posB.z;
			const float Cw = 1.0f / posC.z;
			const Type  aAw = atrA / posA.z;
			const Type  aBw = atrB / posB.z;
			const Type  aCw = atrC / posC.z;
			const float eAREA = edgeFunction2D(posC, posB, posA);

			///------- process 2 triangles

			const trg_half TRG[] = {
				trg_half(posA, posD, posA, posB),
				trg_half(posD, posC, posB, posC)
			};

			for (int side = 0; side < ARRAY_SIZE(TRG); side++)
			{
				const trg_half& triangle = TRG[side];

				for (int i = 0; i <= triangle.iter_count; i++)
				{
					const float iy  = triangle.AA.y + i;
					const float dim = float(i) / triangle.iter_count;

					int istart = (int)lerp(triangle.AA.x, triangle.AB.x, dim);
					int iend   = (int)lerp(triangle.BA.x, triangle.BB.x, dim);

					if (istart > iend)
						std::swap(istart, iend);

					for (int ix = istart; ix <= iend; ix++)
					{
						const FLOAT3 PT = { float(ix), float(iy), 0 };

						const bool pt_in =
							PT.x >= 0 &&
							PT.y >= 0 &&
							PT.x < surface.size().x &&
							PT.y < surface.size().y;

						if (!pt_in)
							continue;

						const float eBA = edgeFunction2D(posB, posA, PT) / eAREA;
						const float eAC = edgeFunction2D(posA, posC, PT) / eAREA;
						const float eCB = edgeFunction2D(posC, posB, PT) / eAREA;

						const INT2 pos = { (int)PT.x,(int)PT.y };

						// slow but coorect borders
						// mb off this or change border AntiA
						//if (eBA >= 0 && eAC >= 0 && eCB >= 0)
						{
							const float z = 1.0f / (eBA * Cw + eAC * Bw + eCB * Aw);
							const Type ATR = ((aCw * eBA) + (aBw * eAC) + (aAw * eCB)) * z;

							if (depthbuffer)
							{

								if (*depthbuffer->raw_data(pos) >= z)
								{
									*depthbuffer->raw_data(pos) = z;
									*surface.raw_data(pos) = sample(ATR);
								}
							}
							else
							{
								*surface.raw_data(pos) = sample(ATR);
							}
						}
					}
				}
			}

			if (wire)
			{
				constexpr color32 wire_с = { 255, 220, 90, 255 };

				posA.x = int(posA.x) + 0.5f;
				posB.x = int(posB.x) + 0.5f;
				posC.x = int(posC.x) + 0.5f;
				posD.x = int(posD.x) + 0.5f;
				posA.y = int(posA.y) + 0.5f;
				posB.y = int(posB.y) + 0.5f;
				posC.y = int(posC.y) + 0.5f;
				posD.y = int(posD.y) + 0.5f;

				drawline2D(surface, { posA.x, posA.y }, { posB.x, posB.y }, wire_с, wire_с);
				drawline2D(surface, { posB.x, posB.y }, { posC.x, posC.y }, wire_с, wire_с);
				drawline2D(surface, { posA.x, posA.y }, { posC.x, posC.y }, wire_с, wire_с);
				drawline2D(surface, { posB.x, posB.y }, { posD.x, posD.y }, wire_с, wire_с);
			}
		}

		template<typename Type>
		inline void drawTriangle3D_opt(
			bitmap32& surface,
			bitmap_t<float>* depthbuffer,
			FLOAT3 posA,
			FLOAT3 posB,
			FLOAT3 posC,
			Type atrA,
			Type atrB,
			Type atrC,
			bool wire = true)
		{
			///------- sort by y-coord

			if (posB.y < posA.y) { std::swap(posB, posA); std::swap(atrB, atrA); }
			if (posB.y > posC.y) { std::swap(posB, posC); std::swap(atrB, atrC); }
			if (posB.y < posA.y) { std::swap(posB, posA); std::swap(atrB, atrA); }

			///------- find D pos

			const float dlp = 1.0f - (posC.y - posB.y) / (posC.y - posA.y);
			FLOAT3 posD(lerp(posA.x, posC.x, dlp), posB.y, 0);

			//-- opt
			const float Aw = 1.0f / posA.z;
			const float Bw = 1.0f / posB.z;
			const float Cw = 1.0f / posC.z;
			const Type  aAw = atrA / posA.z;
			const Type  aBw = atrB / posB.z;
			const Type  aCw = atrC / posC.z;
			const float eAREA = edgeFunction2D(posC, posB, posA);

			///------- process 2 triangles

			const trg_half TRG[] = {
				trg_half(posA, posD, posA, posB),
				trg_half(posD, posC, posB, posC)
			};

			for (int side = 0; side < ARRAY_SIZE(TRG); side++)
			{
				const trg_half& triangle = TRG[side];

				for (int i = 0; i <= triangle.iter_count; i++)
				{
					const float iy  = triangle.AA.y + i;
					const float dim = float(i) / triangle.iter_count;

					int istart = (int)lerp(triangle.AA.x, triangle.AB.x, dim);
					int iend   = (int)lerp(triangle.BA.x, triangle.BB.x, dim);

					if (istart > iend)
						std::swap(istart, iend);

					for (int ix = istart; ix <= iend; ix++)
					{
						const FLOAT3 PT = { float(ix), float(iy), 0 };

						const bool pt_in =
							PT.x >= 0 &&
							PT.y >= 0 &&
							PT.x < surface.size().x &&
							PT.y < surface.size().y;

						if (!pt_in)
							continue;

						const float eBA = edgeFunction2D(posB, posA, PT) / eAREA;
						const float eAC = edgeFunction2D(posA, posC, PT) / eAREA;
						const float eCB = edgeFunction2D(posC, posB, PT) / eAREA;

						const INT2 pos = { (int)PT.x,(int)PT.y };

						// slow but coorect borders
						// mb off this or change border AntiA
						//if (eBA >= 0 && eAC >= 0 && eCB >= 0)
						{
							const float z = 1.0f / (eBA * Cw + eAC * Bw + eCB * Aw);
							const Type ATR = ((aCw * eBA) + (aBw * eAC) + (aAw * eCB)) * z;
							*surface.raw_data(pos) = sample(ATR);
						}
					}
				}
			}
		}
	}
}

LIB_NAMESPACE_END

using namespace art;
using namespace art::draw;



