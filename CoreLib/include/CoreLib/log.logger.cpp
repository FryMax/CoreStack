#include "globals.h"

#if (EXTERN_LOGGER)
	#pragma warning(push)
	#pragma warning(disable : 26439)
	#pragma warning(disable : 26451)
	#pragma warning(disable : 26495)
	#pragma warning(disable : 26812)

	#include <spdlog/spdlog.h>
	#include <spdlog/sinks/stdout_color_sinks.h>
	#pragma warning(pop)

	class SpdLogger : NonAssignable
	{
	private:
		std::shared_ptr<spdlog::logger> console;

	private:
		SpdLogger()
		{
			console = spdlog::stdout_color_mt("console");
			console->set_level(spdlog::level::debug);
		}

	public:
		void Error(const std::string& arg) { console->error(arg); }
		void Warn (const std::string& arg) { console->warn(arg);  }
		void Info (const std::string& arg) { console->info(arg);  }
		void Debug(const std::string& arg) { console->debug(arg); }
		void Trace(const std::string& arg) { console->trace(arg); }

		static SpdLogger* inst()
		{
			static SpdLogger log;
			return &log;
		}
	};

	void link_helper_error(const std::string& txt) { SpdLogger::inst()->Error(txt); }
	void link_helper_warn (const std::string& txt) { SpdLogger::inst()->Warn(txt);  }
	void link_helper_info (const std::string& txt) { SpdLogger::inst()->Info(txt);  }
	void link_helper_debug(const std::string& txt) { SpdLogger::inst()->Debug(txt); }
	void link_helper_trace(const std::string& txt) { SpdLogger::inst()->Trace(txt); }
#endif

LIB_NAMESPACE_START

namespace log
{
	enum class todo_log_level //todo mb dell
	{
		lvl_error,
		lvl_warn,
		lvl_info,
		lvl_debug,
		lvl_trace,
	};

	logger error;
	logger warn;
	logger info;
	logger debug;
	logger trace;

	logger& operator<<(logger& llog, const std::string& text)
	{
		if (!llog.link)
		{
			update_output_links(); // shoud be called once only

			if (!llog.link) // make sure that log impl defined
				throw std::runtime_error("bad log");
		}

		llog.link(text);
		return llog;
	}

	//
	//  Note:
	//    Be sure that you call this after output redirection
	//
	bool update_output_links()
	{
		static int call_count = 0;
		if (call_count++)
		{
			printf("warn: " __FUNCTION__ " multicalled: %d\n", call_count);
			BREAK();
		}

#if (EXTERN_LOGGER)
		log::error.link = &link_helper_error;
		log::warn.link  = &link_helper_warn;
		log::info.link  = &link_helper_info;
		log::debug.link = &link_helper_debug;
		log::trace.link = &link_helper_trace;
#else
		log::error.link = [](const std::string& msg) -> void { printf("ERROR: %s\n", msg.c_str()); };
		log::warn.link  = [](const std::string& msg) -> void { printf(" WARN: %s\n", msg.c_str()); };
		log::info.link  = [](const std::string& msg) -> void { printf(" INFO: %s\n", msg.c_str()); };
		log::debug.link = [](const std::string& msg) -> void { printf("DEBUG: %s\n", msg.c_str()); };
		log::trace.link = [](const std::string& msg) -> void { printf("TRACE: %s\n", msg.c_str()); };
#endif

		return true;
	}
}

LIB_NAMESPACE_END