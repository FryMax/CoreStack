#pragma once

#include "globals.h"
#include "math.types.h"
#include "math.algo.h"

LIB_NAMESPACE_START

namespace art
{
	namespace color
	{
		using color3f = FLOAT3;
		using color4f = FLOAT4;

		struct color24
		{
			byte_t r;
			byte_t g;
			byte_t b;

			constexpr color24(byte_t R = 0, byte_t G = 0, byte_t B = 0) : r(R), g(G), b(B) {}
			constexpr color24(uint32_t hex) :
				r(static_cast<byte_t>(hex >> 16)),
				g(static_cast<byte_t>(hex >> 8)),
				b(static_cast<byte_t>(hex))
			{
				//todo bigendian
				DEBUG_BREAK_IF(hex > 0x00'FF'FF'FFu);
			}
		};

		struct color32
		{
			byte_t r;
			byte_t g;
			byte_t b;
			byte_t a;

			constexpr color32(byte_t R = 0, byte_t G = 0, byte_t B = 0, byte_t A = 0) : r(R), g(G), b(B), a(A) {}
			constexpr color32(uint32_t hex) :
				r(static_cast<byte_t>(hex >> 24)),
				g(static_cast<byte_t>(hex >> 16)),
				b(static_cast<byte_t>(hex >> 8)),
				a(static_cast<byte_t>(hex))
			{}
		};

		namespace palette
		{
			constexpr color32 BLACK = color32(0x00'00'00'FFu);
			constexpr color32 WHITE = color32(0xFF'FF'FF'FFu);
			constexpr color32 RED   = color32(0xFF'00'00'FFu);
			constexpr color32 GRAY  = color32(0xD0'D0'D0'FFu);
		}

		//----------------------------------------------------------------
		//

		namespace
		{
			//todo mb use std::clamp? (not supported yet)
			constexpr byte_t clip_u8(int32_t X)
			{
				//return X > 255 ? 255 : X < 0 ? 0 : X;
				return std::max(0, std::min(X, 255));
			}
		}

		constexpr color32 to_color32(const color4f& col)
		{
			return color32{
				clip_u8(static_cast<int32_t>(col.x * 255.f)),
				clip_u8(static_cast<int32_t>(col.y * 255.f)),
				clip_u8(static_cast<int32_t>(col.z * 255.f)),
				clip_u8(static_cast<int32_t>(col.w * 255.f)),
			};
		}

		//todo const constexpr case em err
		inline color32 random_color32()
		{
			return color32{
				static_cast<uint8_t>(math::xorshiftf32()),
				static_cast<uint8_t>(math::xorshiftf32()),
				static_cast<uint8_t>(math::xorshiftf32()),
				255,
			};
		}

		constexpr bool operator==(const color24& A, const color24& B)
		{
			return A.r == B.r
				&& A.g == B.g
				&& A.b == B.b;
		}

		constexpr bool operator==(const color32& A, const color32& B)
		{
			return A.r == B.r
				&& A.g == B.g
				&& A.b == B.b
				&& A.a == B.a;
		}

		//--------------// RGB to ...

		constexpr color24 toYUV(int R, int G, int B)
		{
			std::uint8_t Y = clip_u8(((+  66 * R + 129 * G +  25 * B + 128) >> 8) +  16);
			std::uint8_t U = clip_u8(((-  38 * R -  74 * G + 112 * B + 128) >> 8) + 128);
			std::uint8_t V = clip_u8(((+ 112 * R -  94 * G -  18 * B + 128) >> 8) + 128);

			return color24(Y, U, V);
		}
		constexpr color24 toYCbCr(int R, int G, int B)
		{
			int _R = 19595 * R;
			int _G = 38470 * G;
			int _B =  7471 * B;

			std::uint8_t Y  = clip_u8(                      (_R + _G + _B) >> 16);
			std::uint8_t Cb = clip_u8((36962 * (B - clip_u8((_R + _G + _B) >> 16)) >> 16) + 128);
			std::uint8_t Cr = clip_u8((46727 * (R - clip_u8((_R + _G + _B) >> 16)) >> 16) + 128);

			return color24(Y, Cb, Cr);
		}

		//--------------// ... to RGB

		constexpr color24 YUV_toRGB(int Y, int U, int V)
		{
			int C = Y -  16;
			int D = U - 128;
			int E = V - 128;
			int C_ = 298 * C;

			std::uint8_t R = clip_u8((C_           + 409 * E + 128) >> 8);
			std::uint8_t G = clip_u8((C_ - 100 * D - 208 * E + 128) >> 8);
			std::uint8_t B = clip_u8((C_ + 516 * D           + 128) >> 8);

			return color24(R, G, B);
		}

		constexpr color24 YCbCr_toRGB(int Y, int Cb, int Cr)
		{
 			std::uint8_t R = clip_u8((Y + ((91881  * Cr             ) >> 16)) - 179);
			std::uint8_t G = clip_u8((Y - ((22544  * Cb + 46793 * Cr) >> 16)) + 135);
			std::uint8_t B = clip_u8((Y + ((116129 * Cb             ) >> 16)) - 226);

			return color24(R, G, B);
		}
	}
}

LIB_NAMESPACE_END