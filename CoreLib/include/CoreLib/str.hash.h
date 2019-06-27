#pragma once

#include "globals.h"

LIB_NAMESPACE_START

namespace str
{
	namespace hash
	{
		static inline std::string sha256(const std::string&, const std::string& = "") { LOG_WARN_NO_IMPL; return ""; };
		static inline std::string md5   (const std::string&, const std::string& = "") { LOG_WARN_NO_IMPL; return ""; };
		static inline std::string xxhash(const std::string&, const std::string& = "") { LOG_WARN_NO_IMPL; return ""; };
	}
}

LIB_NAMESPACE_END