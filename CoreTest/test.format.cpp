#include "pch.h"
#include "google_test_shared.h"
using namespace LIB_NAME;
#define TestGroup Format

namespace
{
	template<typename... Args>
	constexpr auto FMT_CMP(const std::string& fmt_str, const std::string& exp_str, Args&&...args)
	{
		const std::string result_str = str::format::insert(fmt_str, std::forward<Args>(args)...);
		EXPECT_EQ(result_str, exp_str);
	}

	TEST(TestGroup, Empty)
	{
		EXPECT_EQ(str::format::insert("", 0), "");
	}

	TEST(TestGroup, FormatInt)
	{
		FMT_CMP("{}", "0", 0);
		FMT_CMP("{}", "5", +5);
		FMT_CMP("{}", "-6", -6);
		FMT_CMP("{}", "-2147483648", std::numeric_limits<int32_t>::min());
		FMT_CMP("{}", "2147483647", std::numeric_limits<int32_t>::max());
		FMT_CMP("{}", "-9223372036854775808", std::numeric_limits<int64_t>::min());
		FMT_CMP("{}", "9223372036854775807", std::numeric_limits<int64_t>::max());
	}

	TEST(TestGroup, FormatReal)
	{
        FMT_CMP("{}", "0.0", 0.);
        FMT_CMP("{}", "0.5", 0.5);
        FMT_CMP("{}", "-0.5", -0.5);
        FMT_CMP("{}", "999.999", 999.999);
		FMT_CMP("{:.0f}", "123", 123.456789);
		FMT_CMP("{:.1f}", "123.5", 123.456789);
		FMT_CMP("{:.2f}", "123.46", 123.456789);
		FMT_CMP("{:.3f}", "123.457", 123.456789);
		FMT_CMP("{:.4f}", "123.4568", 123.456789);
		FMT_CMP("{:.5f}", "123.45679", 123.456789);
		FMT_CMP("{:.6f}", "123.456789", 123.456789);
	}

	TEST(TestGroup, Align)
	{
		FMT_CMP("{:>3}{:<3}", "  []  ", "[", "]");
	}

}