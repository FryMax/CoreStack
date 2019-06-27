#pragma once

#include "globals.h"

LIB_NAMESPACE_START

struct INT2
{
	int32_t x;
	int32_t y;

	constexpr INT2() : x(0), y(0) {}

	template<typename iType>
	constexpr INT2(iType _x, iType _y) : x(static_cast<int32_t>(_x)), y(static_cast<int32_t>(_y))
	{
		static_assert(std::is_convertible<iType, int32_t>::value, "bad type cast");
	}

	template<typename iTypeA, typename iTypeB>
	constexpr INT2(iTypeA _x, iTypeB _y) : x(static_cast<int32_t>(_x)), y(static_cast<int32_t>(_y))
	{
		static_assert(std::is_convertible<iTypeA, int32_t>::value, "bad type cast");
		static_assert(std::is_convertible<iTypeB, int32_t>::value, "bad type cast");
	}
};

constexpr INT2 operator+ (const INT2& a, const INT2& b) { return INT2(a.x + b.x, a.y + b.y); }
constexpr INT2 operator- (const INT2& a, const INT2& b) { return INT2(a.x - b.x, a.y - b.y); }
constexpr INT2 operator* (const INT2& a, const INT2& b) { return INT2(a.x * b.x, a.y * b.y); }
constexpr INT2 operator/ (const INT2& a, const INT2& b) { return INT2(a.x / b.x, a.y / b.y); }

constexpr INT2 operator* (const INT2& a, int b)   { return INT2(a.x * b, a.y * b); }
constexpr INT2 operator/ (const INT2& a, int b)   { return INT2(a.x / b, a.y / b); }
constexpr INT2 operator* (const INT2& a, float b) { return INT2(a.x * b, a.y * b); }
constexpr INT2 operator/ (const INT2& a, float b) { return INT2(a.x / b, a.y / b); }

constexpr bool operator!=(const INT2& a, const INT2& b) { return (a.x != b.x || a.y != b.y); }
constexpr bool operator==(const INT2& a, const INT2& b) { return (a.x == b.x && a.y == b.y); }

constexpr INT2& operator+=(INT2& a, const INT2& b) { a.x += b.x; a.y += b.y; return a; }
constexpr INT2& operator-=(INT2& a, const INT2& b) { a.x -= b.x; a.y -= b.y; return a; }

constexpr INT2& operator*=(INT2& a, int b)   { a.x *= b; a.y *= b; return a; }
constexpr INT2& operator/=(INT2& a, int b)   { a.x /= b; a.y /= b; return a; }
constexpr INT2& operator*=(INT2& a, float b) { a.x *= static_cast<int>(b); a.y *= static_cast<int>(b); return a; }
constexpr INT2& operator/=(INT2& a, float b) { a.x /= static_cast<int>(b); a.y /= static_cast<int>(b); return a; }

struct FLOAT2
{
	float x;
	float y;
	constexpr FLOAT2() : x(0.0f), y(0.0f) {}
	constexpr FLOAT2(float _x, float _y) : x(_x), y(_y) {}

	//TODO mb delete one of them
	constexpr float  operator[] (size_t i) const { BREAK_IF(i >= 2); return (&x)[i]; }
	constexpr float& operator[] (size_t i)       { BREAK_IF(i >= 2); return (&x)[i]; }
};

struct FLOAT3
{
	float x; float y; float z;
	constexpr FLOAT3() : x(0.0f), y(0.0f), z(0.0f) {}
	constexpr FLOAT3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct FLOAT4
{
	float x; float y; float z; float w;
	constexpr FLOAT4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	constexpr FLOAT4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

struct VECTOR {};
struct MATRIX {};

using CINT2   = const INT2&;
using CFLOAT2 = const FLOAT2&;
using CFLOAT3 = const FLOAT3&;
using CFLOAT4 = const FLOAT4&;
using CVECTOR = const VECTOR&;
using CMATRIX = const MATRIX&;

// FLOAT2
constexpr inline FLOAT2  operator+  (CFLOAT2 a, CFLOAT2 b)     { return FLOAT2(a.x + b.x, a.y + b.y); }
constexpr inline FLOAT2  operator-  (CFLOAT2 a, CFLOAT2 b)     { return FLOAT2(a.x - b.x, a.y - b.y); }
constexpr inline FLOAT2  operator*  (CFLOAT2 a, CFLOAT2 b)     { return FLOAT2(a.x * b.x, a.y * b.y); }
constexpr inline FLOAT2  operator/  (CFLOAT2 a, CFLOAT2 b)     { return FLOAT2(a.x / b.x, a.y / b.y); }
constexpr inline FLOAT2  operator*  (CFLOAT2 a, const float b) { return FLOAT2(a.x * b, a.y * b);     }
constexpr inline FLOAT2  operator/  (CFLOAT2 a, const float b) { return FLOAT2(a.x / b, a.y / b);     }
constexpr inline FLOAT2& operator+= (FLOAT2& a, CFLOAT2 b)     { a.x += b.x; a.y += b.y; return a;    }
constexpr inline FLOAT2& operator-= (FLOAT2& a, CFLOAT2 b)     { a.x -= b.x; a.y -= b.y; return a;    }
constexpr inline FLOAT2& operator*= (FLOAT2& a, const float b) { a.x *= b;   a.y *= b;   return a;    }
constexpr inline FLOAT2& operator/= (FLOAT2& a, const float b) { a.x /= b;   a.y /= b;   return a;    }

// FLOAT3
constexpr inline FLOAT3  operator+  (CFLOAT3 a, CFLOAT3 b)      { return FLOAT3(a.x + b.x, a.y + b.y, a.z + b.z); }
constexpr inline FLOAT3  operator-  (CFLOAT3 a, CFLOAT3 b)      { return FLOAT3(a.x - b.x, a.y - b.y, a.z - b.z); }
constexpr inline FLOAT3  operator*  (CFLOAT3 a, CFLOAT3 b)      { return FLOAT3(a.x * b.x, a.y * b.y, a.z * b.z); }
constexpr inline FLOAT3  operator/  (CFLOAT3 a, CFLOAT3 b)      { return FLOAT3(a.x / b.x, a.y / b.y, a.z / b.z); }
constexpr inline FLOAT3  operator*  (CFLOAT3 a, const float b)  { return FLOAT3(a.x * b, a.y * b, a.z * b);       }
constexpr inline FLOAT3  operator/  (CFLOAT3 a, const float b)  { return FLOAT3(a.x / b, a.y / b, a.z / b);       }
constexpr inline FLOAT3& operator+= (FLOAT3& a, CFLOAT3 b)      { a.x += b.x; a.y += b.y;  a.z += b.z; return a;  }
constexpr inline FLOAT3& operator-= (FLOAT3& a, CFLOAT3 b)      { a.x -= b.x; a.y -= b.y;  a.z -= b.z; return a;  }
constexpr inline FLOAT3& operator*= (FLOAT3& a, const float b)  { a.x *= b;   a.y *= b;    a.z *= b;   return a;  }
constexpr inline FLOAT3& operator/= (FLOAT3& a, const float b)  { a.x /= b;   a.y /= b;    a.z /= b;   return a;  }

// FLOAT4
constexpr inline FLOAT4  operator+  (CFLOAT4 a, CFLOAT4 b)      { return FLOAT4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
constexpr inline FLOAT4  operator-  (CFLOAT4 a, CFLOAT4 b)      { return FLOAT4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
constexpr inline FLOAT4  operator*  (CFLOAT4 a, CFLOAT4 b)      { return FLOAT4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
constexpr inline FLOAT4  operator/  (CFLOAT4 a, CFLOAT4 b)      { return FLOAT4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
constexpr inline FLOAT4  operator*  (CFLOAT4 a, const float b)  { return FLOAT4(a.x * b,   a.y * b,   a.z * b,   a.w * b);   }
constexpr inline FLOAT4  operator/  (CFLOAT4 a, const float b)  { return FLOAT4(a.x / b,   a.y / b,   a.z / b,   a.w / b);   }
constexpr inline FLOAT4& operator+= (FLOAT4& a, CFLOAT4 b)      { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w; return a;  }
constexpr inline FLOAT4& operator-= (FLOAT4& a, CFLOAT4 b)      { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w; return a;  }
constexpr inline FLOAT4& operator*= (FLOAT4& a, const float b)  { a.x *= b;   a.y *= b;   a.z *= b;   a.w *= b;   return a;  }
constexpr inline FLOAT4& operator/= (FLOAT4& a, const float b)  { a.x /= b;   a.y /= b;   a.z /= b;   a.w /= b;   return a;  }

// TODO mb dell
//constexpr inline bool operator>(CFLOAT4 a, CFLOAT4 b) { return (a.x > b.x && a.y > b.y && a.z > b.z); } // no (w/alpha) component compare !
//constexpr inline bool operator<(CFLOAT4 a, CFLOAT4 b) { return (a.x < b.x && a.y < b.y && a.z < b.z); } // no (w/alpha) component compare !


// NOTE: mb rename to INT4 or RECT
template<typename t, class ptType>
struct quad_t
{
public:
	constexpr quad_t(t _left, t _top, t _right, t _bottom) :
		left(_top),
		top(_left),
		right(_right),
		bottom(_bottom)
	{}

	constexpr quad_t(ptType _pos, ptType _size) :
		left(_pos.x),
		top(_pos.y),
		right(_pos.x + _size.x),
		bottom(_pos.y + _size.y)
	{}

	constexpr quad_t() :
		left(0),
		top(0),
		right(0),
		bottom(0)
	{}

	// get

	ptType GetPos () const { return ptType(left, top); };
	ptType GetSize() const { return ptType(right - left, bottom - top); };
	ptType GetA   () const { return GetPos(); };
	ptType GetB   () const { return GetPos() + GetSize(); };

	bool isContains(ptType point) const
	{
		return (point.x > left)
			&& (point.x < right)
			&& (point.y > top)
			&& (point.y < bottom);
	}

	bool isInverted() const
	{
		return (left > right) || (top > bottom);
	}

	// set

	void normilize()
	{
		if (left > right) std::swap(left, right);
		if (bottom > top) std::swap(left, right);
	}

public:
	t top = 0;
	t left = 0;
	t right = 0;
	t bottom = 0;
};

using QUAD  = quad_t<int, INT2>;
using QUADf = quad_t<float, FLOAT2>;

LIB_NAMESPACE_END