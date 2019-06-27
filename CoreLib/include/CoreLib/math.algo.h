#pragma once

#include "globals.h"
#include "math.types.h"

LIB_NAMESPACE_START

namespace math
{
    // todo:
    // need tests

	namespace constants
	{
		constexpr double PI  = 3.1415926535;
		constexpr double RAD = PI / 180.;
		constexpr double E   = 2.7182818284;
		constexpr double G   = 9.8066;

	} // namespace constants

	namespace helpers
	{
		static const int stdlib_random_seed_dummy = (std::srand(unsigned(std::time(NULL) ^ std::random_device()())), 0);

		constexpr uint32_t  xorshift32_seed[] = { 2463534242 };
		constexpr uint64_t  xorshift64_seed[] = { 88172645463325252 };
		constexpr uint32_t  xorshift96_seed[] = { 123456789, 362436069, 521288629 };
		constexpr uint32_t xorshift128_seed[] = { 123456789, 362436069, 521288629, 88675123 };
		//--------------------------------------------------------------------------------------------------------------------------// xor-shiftf random generators (fastest)

		// period: 4294967296 (2^32-1)
		inline uint32_t xorshiftf32(void)
		{
			static uint32_t y = xorshift32_seed[0];
			y = y ^ (y << 13);
			y = y ^ (y >> 17);
			return y = y ^ (y << 15);
		}

		// period: 18446744073709550000 (2^64-1)
		inline uint32_t xorshiftf64(void)
		{
			static uint64_t x = xorshift64_seed[0];
			x = x ^ (x << 13);
			x = x ^ (x >> 7);
			return uint32_t(x = x ^ (x << 17));
		}

		// period: (2^96-1)
		inline uint32_t xorshiftf96(void)
		{
			static uint32_t x = xorshift96_seed[0];
			static uint32_t y = xorshift96_seed[1];
			static uint32_t z = xorshift96_seed[2];
			uint32_t t = (x ^ (x << 3)) ^ (y ^ (y >> 19)) ^ (z ^ (z << 6));
			x = y;
			y = z;
			return z = t;
		}

		// period: (2^128-1)
		inline uint32_t xorshift128(void)
		{
			static uint32_t x = xorshift128_seed[0];
			static uint32_t y = xorshift128_seed[1];
			static uint32_t z = xorshift128_seed[2];
			static uint32_t w = xorshift128_seed[3];
			uint32_t t = x ^ (x << 11);
			x = y;
			y = z;
			z = w;
			return (w ^ (w >> 19)) ^ (t ^ (t >> 8));
		}

		// period: 18446744073709550000 (2^64-1) (faster than xorshiftf64 by 1 operation on a x64 machine)
		inline uint64_t xorshiftf64_64(void)
		{
			static uint64_t x = xorshift64_seed[0];
			x = x ^ (x << 7);
			return x = x ^ (x >> 9);
		}

		inline double xorshift_rand_real(const double A, const double B)
		{
			constexpr uint64_t MANT_MASK53(0x1FFFFFFFFFFFFF);
			constexpr double ITO_D53(1.0 / 0x20000000000000);

			const uint64_t x(xorshiftf64_64() & MANT_MASK53);
			return A + ITO_D53 * static_cast<double>(x)* (B - A);
		}

		namespace
		{
			inline uint64_t rand_dev_helper()
			{
				if(sizeof(decltype(std::random_device()()))==sizeof(uint64_t))
					 return (static_cast<uint64_t>(std::random_device()()));
				else return (static_cast<uint64_t>(std::random_device()()) << 32) | std::random_device()();
			}
		}

		inline uint64_t rand64(void)
		{
			static uint64_t x = xorshift64_seed[0];
			static uint64_t y = rand_dev_helper();

			x = x ^ (x << 7);
			x = x ^ (x >> 9);
			return x ^ y;
		}

		//----------- todo make it

		inline uint64_t xorshift_64x64()            { return xorshift64_seed[0]; }
		inline uint64_t xorshift_64x64(uint64_t& x) { x = x ^ (x << 7); x = x ^ (x >> 9); return x; }

		//--------------------------------------------------------------------------------------------------------------------------// stdlib random generators (fast)

		inline void stdlib_rand_set_seed(unsigned seed = std::random_device()())
		{
			std::srand(seed);
		}

 		inline int32_t  stdlib_rand_ui  () { uint32_t r = 0; ForArray(i, 3) r = (r << 15) | (rand() & RAND_MAX); return r & UINT32_MAX; }
		inline uint64_t stdlib_rand_ull () { uint64_t r = 0; ForArray(i, 5) r = (r << 15) | (rand() & RAND_MAX); return r & UINT64_MAX; }
 		inline double   stdlib_rand_dbl (const double range_min, const double range_max)
		{
			static const uint64_t mant_mask53(9007199254740991);
			static const double	  i_to_d53(1.0 / 9007199254740992.0);
			const uint64_t r((uint64_t(rand()) << 00  |
							 (uint64_t(rand()) << 15) |
							 (uint64_t(rand()) << 30) |
							 (uint64_t(rand()) << 45)) & mant_mask53);
			return range_min + i_to_d53*double(r)*(range_max - range_min);
		}

		//--------------------------------------------------------------------------------------------------------------------------// std random generators algorithms (usualy slow)

		template<typename Generator>
		struct StdRandomGenerator
		{

		private:
			std::mt19937_64 TGenerator;

		// std rand gens:
		//
		//		default_random_engine - mt19937_64
		//		minstd_rand0          - linear_congruential_engine
		//		minstd_rand           - linear_congruential_engine
		//		mt19937               - mersenne_twister_engine
		//		mt19937_64            - mersenne_twister_engine
		//		ranlux24_base	      - subtract_with_carry_engine
		//		ranlux48_base         - subtract_with_carry_engine
		//		ranlux24              - discard_block_engine
		//		ranlux48              - discard_block_engine
		//		knuth_b               - shuffle_order_engine

		public:

			std::string get_algo_name()
			{
				return typeid(TGenerator).name();
			}

			//--------------------------------------------------------------------------------------------------// seed

			void set_random_seed()
			{
				std::vector<decltype(std::random_device()())> seq;

				ForArray(i, 0xFULL + std::random_device()() % 0xFULL)
					seq.push_back(std::random_device()());

				//TODO em error
				// TGenerator.seed(std::seed_seq(seq.begin(), seq.end()));
			}

			inline void set_seed_default() { TGenerator = Generator(); }
			inline void set_seed(int64_t seed) { TGenerator.seed(seed); }
			inline void set_seed(const std::string& seed) { TGenerator.seed(std::seed_seq(seed.begin(), seed.end())); }

			template<typename I>
			void set_seed(I iseq) { TGenerator.seed(std::seed_seq(iseq.begin(), iseq.end())); }

			//--------------------------------------------------------------------------------------------------// generate

			inline decltype(Generator()()) generate() { return TGenerator(); }

			inline int64_t generateInt(int64_t min, int64_t max)
			{
				BREAK_IF(min > max);
				std::uniform_int_distribution<int64_t> dis(min, max);
				return dis(TGenerator);
			}

			inline double generateReal(double min, double max)
			{
				BREAK_IF(min > max);
				std::uniform_real_distribution<double> dis(min, max);
				return dis(TGenerator);
			}

		};

		namespace
		{
			inline StdRandomGenerator<std::mt19937_64>& _GetSharedGen()
			{
				static StdRandomGenerator<std::mt19937_64> shared_gen;
				shared_gen.set_seed(int64_t(std::random_device()()));

				MakeOnce
				{
					shared_gen.set_random_seed();
				}

				return shared_gen;
			}

			inline int64_t GenRandInt64(int64_t min, int64_t max) { return _GetSharedGen().generateInt (min, max); }
			inline double  GenRandReal (double  min, double  max) { return _GetSharedGen().generateReal(min, max); }
		}

	} // namespace helpers

	//--------------------------------------------------------------------------------------------------// math_helpers end
	namespace
	{
		using namespace math::helpers;
		using namespace math::constants;
	}

	//------ slow, good entropy

	inline int64_t RandomInt64 (int64_t A, int64_t B) { return GenRandInt64 (A, B); }
	inline double  RandomDouble(double  A, double  B) { return GenRandReal  (A, B); }

	//------ ok, mb delete

	inline int32_t RandomStd32   (int32_t A, int32_t B) { return A + (stdlib_rand_ui ()) % int32_t(B - A + 1); }
	inline int64_t RandomStd64   (int64_t A, int64_t B) { return A + (stdlib_rand_ull()) % int64_t(B - A + 1); }
	inline double  RandomStdReal (double  A, double  B) { return stdlib_rand_dbl(A, B); }

	//------ fast

	inline int32_t RandomFastInt32(int32_t A, int32_t B) { return A + (xorshiftf32())    % int32_t(B - A + 1); }
	inline int64_t RandomFastInt64(int64_t A, int64_t B) { return A + (xorshiftf64_64()) % int64_t(B - A + 1); }
	inline double  RandomFastReal (double  A, double  B) { return xorshift_rand_real(A, B); }

	//==========================================================================// rand

	using RandomGenerator = StdRandomGenerator<std::mt19937_64>;

	template<typename T_itype>
	inline int64_t Random(T_itype A, T_itype B) { return GenRandInt64(A, B); }
	inline double  Random(float   A, float   B) { return GenRandReal (A, B); }
	inline double  Random(double  A, double  B) { return GenRandReal (A, B); }

	//==========================================================================// EQWALS

	// mb delete
	template<typename T> inline T max(const T &a, const T &b) { return (((a) > (b)) ? (a) : (b)); }
	template<typename T> inline T min(const T &a, const T &b) { return (((a) < (b)) ? (a) : (b)); }

	//==========================================================================//	RANGES

	// mb delete
	template<typename T, typename T1, typename T2>
	constexpr bool InRange(const T& A, const T1& L, const T2& R) { return L <= A && A <= R; }

	template<typename T>
	constexpr bool InRange(const T& A, const T& L, const T& R) { return L <= A && A <= R; }

	// mb delete
	template<typename T>
	constexpr bool InRangeEx(const T a, T a1, T a2, bool include_borders = true, bool require_increase = false)
	{
		const bool swap = !require_increase;
		const bool bord = include_borders;

		if (swap)
		{
			if (a1 > a2)
			{
				T tmp = a2;
				a2 = a1;
				a1 = tmp;
 			}
		}

		if (bord) { return L <= A && A <= R; }
		else	  { return L  < A && A  < R; }
	}





	// in_range strick
	// in_range reverse ranges

	//==========================================================================// equal

	template<typename T>
	inline bool equal(const T	   &a, const T		&b) { return (a == b); }
	inline bool equal(const float  &a, const float  &b) { return (fabs(a - b) <= std::numeric_limits<float> ::epsilon());	}
	inline bool equal(const double &a, const double &b) { return (fabs(a - b) <= std::numeric_limits<double>::epsilon());	}

	inline bool equal(CFLOAT2 A, CFLOAT2 B) { return (equal(A.x, B.x) && equal(A.y, B.y)); }
	inline bool equal(CFLOAT3 A, CFLOAT3 B) { return (equal(A.x, B.x) && equal(A.y, B.y) && equal(A.z, B.z)); }
	inline bool equal(CFLOAT4 A, CFLOAT4 B) { return (equal(A.x, B.x) && equal(A.y, B.y) && equal(A.z, B.z) && equal(A.w, B.w)); }

	//==========================================================================//  equal with epsilon

	template<typename T>
	inline bool equal(const T      &a, const T      &b, const T      &e) { return (std::abs(a - b) <= e); }
	inline bool equal(const float  &a, const float  &b, const float  &e) { return (fabsf   (a - b) <= e); }
	inline bool equal(const double &a, const double &b, const double &e) { return (fabs    (a - b) <= e); }

	inline bool equal(CFLOAT4 A, CFLOAT4 B, const float& e) { return(equal(A.x, B.x, e) &&equal(A.y, B.y, e) &&equal(A.z, B.z, e) &&equal(A.w, B.w, e)); }

	//==========================================================================//  consists

	constexpr int64_t fib_number(int n)
	{
		int64_t x = 1;
		int64_t	y = 0;
		int64_t ret = 0;

		for (int64_t i = x; i <= n; i++)
		{
			ret = x + y;
			x = y;
			y = ret;
		}

		return ret;
	}

	//==========================================================================//  linear

	inline float cross_product(CFLOAT2 a, CFLOAT2 b, CFLOAT2 c) { return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y); }

	inline float distance_2d(float xa, float ya, float xb, float yb) { return sqrt((xb  -  xa) * (xb  -  xa) + (yb  -  ya) * (yb  -  ya)); }
	inline float distance_2d(CFLOAT2 a, CFLOAT2 b)                   { return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y)); }
	inline float distance_3d(CFLOAT3 a, CFLOAT3 b)                   { return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y) + (b.z - a.z) * (b.z - a.z)); }

	//==========================================================================//

	inline float anlage_2d(CFLOAT2 A, CFLOAT2 B)
	{
		const float Anlage = -atan2f(A.x - B.x, A.y - B.y) / float(PI) * 180.f;
		return (Anlage < 0.f) ? Anlage + 360.f : Anlage;
	}

	inline FLOAT2 move_on_angle(CFLOAT2 pos, float angle360, float dist)
	{
		return{
			pos.x + cos(float(RAD) * (angle360 - 90.f)) * dist,
			pos.y + sin(float(RAD) * (angle360 - 90.f)) * dist,
		};
	}

	inline bool is_point_in_triangle2D(CFLOAT2 p, CFLOAT2 a, CFLOAT2 b, CFLOAT2 c)
	{
		const float fcp_ab = cross_product(p, a, b);
		const float fcp_bc = cross_product(p, b, c);
		const float fcp_ca = cross_product(p, c, a);

		const bool cp_ab = fcp_ab < 0.0f;
		const bool cp_bc = fcp_bc < 0.0f;
		const bool cp_ca = fcp_ca < 0.0f;

		return ((cp_ab == cp_bc) && (cp_bc == cp_ca));
	}

	inline FLOAT3 point_near_triangle_2d(CFLOAT2 p, CFLOAT2 a, CFLOAT2 b, CFLOAT2 c)
	{
		const float fcp_ab = cross_product(p, a, b);
		const float fcp_bc = cross_product(p, b, c);
		const float fcp_ca = cross_product(p, c, a);

		return FLOAT3(fcp_ab, fcp_bc, fcp_ca);
	}

	//------------------------------------------------------------------ interpolation

	constexpr float lerp(float a, float b, float t)
	{
		return (1.0f - t) * a + t * b;
	}

	constexpr double lerp(double a, double b, double t)
	{
		return (1.0 - t) * a + t * b;
	}

};

LIB_NAMESPACE_END