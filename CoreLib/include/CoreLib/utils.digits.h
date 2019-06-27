#pragma once

#include "globals.h"
#include "math.algo.h"

#include <3rd-party\murmur3.h>


namespace
{
	constexpr auto RAND_64_FN = math::rand64;
}

LIB_NAMESPACE_START

	namespace
	{
		//TODO
		//constexpr uint8_t str_to_hex(char c)
		//{
		//	return CHAR_MIN + uint8_t(c);
		//}
	}
	inline std::string byte_to_bits(byte val)
	{
		return std::string
		{
			"01"[1 & (val >> 7)],
			"01"[1 & (val >> 6)],
			"01"[1 & (val >> 5)],
			"01"[1 & (val >> 4)],
			"01"[1 & (val >> 3)],
			"01"[1 & (val >> 2)],
			"01"[1 & (val >> 1)],
			"01"[1 & (val)],
		};
	}

	inline std::string byte_to_bits_alt(byte val)
	{
		return std::string
		{
			(0b10000000 & val) > 0 ? '1' : '0',
			(0b01000000 & val) > 0 ? '1' : '0',
			(0b00100000 & val) > 0 ? '1' : '0',
			(0b00010000 & val) > 0 ? '1' : '0',
			(0b00001000 & val) > 0 ? '1' : '0',
			(0b00000100 & val) > 0 ? '1' : '0',
			(0b00000010 & val) > 0 ? '1' : '0',
			(0b00000001 & val) > 0 ? '1' : '0',
		};
	}

	constexpr char byte_hex_hi(byte val) { return "0123456789ABCDEF"[val >> 4]; }
	constexpr char byte_hex_lo(byte val) { return "0123456789ABCDEF"[val & 15]; }

	// TODO
	// RANGE convert begin end
	inline std::string byte_to_hex_str(byte b)
	{
		return std::string
		{
			byte_hex_hi(b),
			byte_hex_lo(b),
		};
	}

	// TODO: perf test
	// SAMP: "FF" -> 256
	constexpr byte hex_i16_to_byte(const char* pint16)
	{
		const char A = pint16[0];
		const char B = pint16[1];
		char hi = 0;
		char lo = 0;

		DEBUG_CODE(
			if (!(math::InRange(A, '0', '9') || math::InRange(A, 'a', 'f') || math::InRange(A, 'A', 'F')))
				BREAK();
			if (!(math::InRange(B, '0', '9') || math::InRange(B, 'a', 'f') || math::InRange(B, 'A', 'F')))
				BREAK();
		);

		// todo:
		//  use map and switch?

		if (math::InRange(A, '0', '9')) hi = 0x0 + A - '0';
		if (math::InRange(A, 'a', 'f')) hi = 0xA + A - 'a';
		if (math::InRange(A, 'A', 'F')) hi = 0xA + A - 'A';
		if (math::InRange(B, '0', '9')) lo = 0x0 + B - '0';
		if (math::InRange(B, 'a', 'f')) lo = 0xA + B - 'a';
		if (math::InRange(B, 'A', 'F')) lo = 0xA + B - 'A';

		return 0x10 * hi + lo;
	}

	//------------------------------------------------------------------------------
	// hash

	namespace
	{
		inline uint64_t murmur_hash_64(const byte* ptr, const int len)
		{
			DEBUG_BREAK_IF(!ptr);
			DEBUG_BREAK_IF(len == 0);
			uint64_t out[2];
			MurmurHash3_x64_128(ptr, len, 0xabcd, &out);
			return out[0] ^ out[1];
		}

		inline void murmur_hash_128(const byte* ptr, const int len, uint64_t* out128bit)
		{
			DEBUG_BREAK_IF(!ptr);
			DEBUG_BREAK_IF(!out128bit);
			DEBUG_BREAK_IF(len == 0);
			MurmurHash3_x64_128(ptr, len, 0xabcd, out128bit);
		}

		template <class T>
		inline void std_hash_combine(std::size_t& seed, const T& v)
		{
			std::hash<T> hasher;
			seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	}

	namespace math
	{
		inline uint64_t hash64(uint64_t seed)
		{
			return murmur_hash_64(reinterpret_cast<const byte*>(&seed), sizeof(uint64_t));
		}

		inline uint64_t hash64(const byte* ptr, const int len)
		{
			return murmur_hash_64(ptr, len);
		}
	}

	//------------------------------------------------------------------------------
	// uid

	namespace
	{
		constexpr size_t UID_LEN = ARRAY_SIZE("12345678-1234-1234-1234-123456789ABC") - 1;
		constexpr size_t UID_TP[] = { 8, 13, 18, 23 };
		constexpr size_t UID_S[] = { 4, 6, 8, 10, 16 };
	}

	// (Non-iso-compatible)
	struct UID
	{
	protected:
		byte raw_uid[16]{};

	public:
		bool is_null()
		{
			return get_lo() == 0 && get_hi() == 0;
		}

		const uint64_t get_hi() const { return *reinterpret_cast<const uint64_t*>(&raw_uid[0]); }
		const uint64_t get_lo() const { return *reinterpret_cast<const uint64_t*>(&raw_uid[8]); }

		uint64_t& get_hi_mut() { return *reinterpret_cast<uint64_t*>(&raw_uid[0]); }
		uint64_t& get_lo_mut() { return *reinterpret_cast<uint64_t*>(&raw_uid[8]); }

		// Generate size_t hash from uid (x32 != x64 hash from same uid)
		size_t gen_hash() const
		{
			const uint64_t val = gen_hash64();
			if constexpr (BUILD_MODE == ARCH_X64)
			{
				return val;
			}
			else
			{
				size_t a = static_cast<size_t>(val);
				size_t b = static_cast<size_t>(val >> 32);
				return a ^ b;
			}
		}

		// Generate 64bit hash from uid (x32 == x64)
		uint64_t gen_hash64() const
		{
			return murmur_hash_64(&raw_uid[0], sizeof(raw_uid));
		}

		bool operator==(const UID& inst) const
		{
			return get_hi() == inst.get_hi()
				&& get_lo() == inst.get_lo();
		}

		bool operator<(const UID& id) const
		{
			return std::memcmp(raw_uid, id.raw_uid, sizeof(UID::raw_uid)) < 0;
		}

		//--------------------------------------------------------------------//
		// static

	public:
		static UID null_uid()
		{
			UID uid;
			uid.get_hi_mut() = 0;
			uid.get_lo_mut() = 0;
			return uid;
		}

		static inline std::string uid_to_string(const UID& id)
		{
			std::string out;
			out.reserve(UID_LEN);

			for (size_t i = 0; i < 16; i++)
				out += byte_to_hex_str(id.raw_uid[i]);

			for (size_t i : UID_TP)
				out.insert(out.begin() + i, '-');

			return out;
		}

		static inline bool is_valid_uid(const std::string& s)
		{
			if (s.length() != UID_LEN)
				return false;

			for (size_t i : UID_TP)
			{
				if (s[i] != '-')
					return false;
			}
			return std::all_of(s.cbegin(), s.cend(), [](int c)
				{
					return c == '-' || 0 != isxdigit(c);
				});
		}

		static inline UID uid_from_string(const std::string& str)
		{
			if (!is_valid_uid(str))
			{
				DEBUG_BREAK();
				return UID::null_uid();
			}

			UID out = null_uid();
			size_t it = 0;
			for (size_t i = 0; i < str.length(); i++)
			{
				if (str[i] != '-')
					out.raw_uid[it++] = hex_i16_to_byte(&str[i++]);
			}
			return out;
		}

		static inline UID gen_uid(const std::string& str)
		{
			if (str.empty())
				return null_uid();

			UID out;
			murmur_hash_128(reinterpret_cast<const byte*>(str.data()), static_cast<int>(str.size()), &out.get_hi_mut());
			return out;
		}

		static inline UID gen_rand_uid()
		{
			UID out;
			out.get_hi_mut() = RAND_64_FN();
			out.get_lo_mut() = RAND_64_FN();
			return out;
		}
	};

LIB_NAMESPACE_END

namespace std
{
	template <>
	struct hash<UID>
	{
		std::size_t operator()(const UID& k) const
		{
			return k.gen_hash();
		}
	};

}