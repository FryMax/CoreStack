#pragma once
#include "globals.h"
#include <fmt/format.h>

LIB_NAMESPACE_START

namespace str
{
	namespace format
	{
		template <typename... Args>
		inline std::string insert(const std::string& str, Args&&...args)
		{
			return fmt::format(str, std::forward<Args>(args)...);
		}
	}
}

LIB_NAMESPACE_END