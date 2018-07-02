/**
 */
#ifndef __cx_log_log_h__
#define __cx_log_log_h__

#include <cx/cxdefine.hpp>
#include <cx/core/basic_buffer.hpp>
#include <cx/log/source.hpp>

#ifndef CXLOG
#if CX_PLATFORM == CX_P_WINDOWS
#define CXLOG(lv,src_or_tag,msg,...) do{ cx::log::core::instance()->log(lv,src_or_tag,__FILE__,__LINE__,__FUNCTION__,msg,__VA_ARGS__); } while(0)
#define CXDUMP(lv,src_or_tag,buf,sz,msg,...) do{ cx::log::core::instance()->dump(lv,src_or_tag,buf,sz,__FILE__,__LINE__,__FUNCTION__,msg,__VA_ARGS__); } while(0)
#define CXLOG_D(src_or_tag,msg,...) CXLOG(cx::log::level::debug,src_or_tag,msg,__VA_ARGS__)
#define CXLOG_I(src_or_tag,msg,...) CXLOG(cx::log::level::info,src_or_tag,msg,__VA_ARGS__)
#define CXLOG_W(src_or_tag,msg,...) CXLOG(cx::log::level::warn,src_or_tag,msg,__VA_ARGS__)
#define CXLOG_E(src_or_tag,msg,...) CXLOG(cx::log::level::error,src_or_tag,msg,__VA_ARGS__)
#define CXLOG_F(src_or_tag,msg,...) CXLOG(cx::log::level::fatal,src_or_tag,msg,__VA_ARGS__)
#else
#define CXLOG(lv,src_or_tag,msg,...) do{ cx::log::core::instance()->log(lv,src_or_tag,__FILE__,__LINE__,__FUNCTION__,msg,##__VA_ARGS__); } while(0)
#define CXDUMP(lv,src_or_tag,buf,sz,msg,...) do{ cx::log::core::instance()->dump(lv,src_or_tag,buf,sz,__FILE__,__LINE__,__FUNCTION__,msg,##__VA_ARGS__); } while(0)
#define CXLOG_D(src_or_tag,msg,...) CXLOG(cx::log::level::debug,src_or_tag,msg,##__VA_ARGS__)
#define CXLOG_I(src_or_tag,msg,...) CXLOG(cx::log::level::info,src_or_tag,msg,##__VA_ARGS__)
#define CXLOG_W(src_or_tag,msg,...) CXLOG(cx::log::level::warn,src_or_tag,msg,##__VA_ARGS__)
#define CXLOG_E(src_or_tag,msg,...) CXLOG(cx::log::level::error,src_or_tag,msg,##__VA_ARGS__)
#define CXLOG_F(src_or_tag,msg,...) CXLOG(cx::log::level::fatal,src_or_tag,msg,##__VA_ARGS__)
#endif // CX_PLATFORM == CX_P_WINDOWS
#endif // CXLOG

namespace cx::log {

	enum class level { debug, info, warn, error, fatal };

	struct record {
		cx::log::level level;
		cx::log::source* source;
		const char* file;
		int line;
		const char* function;
		std::chrono::system_clock::time_point time;
		std::thread::id tid;
		cx::basic_buffer<char> message;

		record(cx::log::level lv,
			cx::log::source* source,
			const char *afile,
			const int aline,
			const char *afunction)
			: level(lv),
			source(source),
			file(afile),
			line(aline),
			function(afunction),
			message(256)
		{
			time = std::chrono::system_clock::now();
			tid = std::this_thread::get_id();
		}
	};
}

#endif