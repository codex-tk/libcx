/**
 */
#ifndef __cx_log_log_h__
#define __cx_log_log_h__

#include <cx/cxdefine.hpp>
#include <cx/core/basic_buffer.hpp>

#ifndef CXLOG
#if CX_PLATFORM == CX_P_WINDOWS
#define CXLOG(lv,tag,msg,...) do{ cx::log::core::instance()->log(lv,__FILE__,__LINE__,__FUNCTION__,tag,msg,__VA_ARGS__); } while(0)
#define CXLOG_D(tag,msg,...) CXLOG(cx::log::level::debug,tag,msg,__VA_ARGS__)
#define CXLOG_I(tag,msg,...) CXLOG(cx::log::level::info,tag,msg,__VA_ARGS__)
#define CXLOG_W(tag,msg,...) CXLOG(cx::log::level::warn,tag,msg,__VA_ARGS__)
#define CXLOG_E(tag,msg,...) CXLOG(cx::log::level::error,tag,msg,__VA_ARGS__)
#define CXLOG_F(tag,msg,...) CXLOG(cx::log::level::fatal,tag,msg,__VA_ARGS__)
#define CXDUMP(lv,tag,buf,sz,msg,...) do{ cx::log::core::instance()->dump(lv,__FILE__,__LINE__,__FUNCTION__,tag,buf,sz,msg,__VA_ARGS__); } while(0)
#else
#define CXLOG(lv,tag,msg,...) do{ cx::log::core::instance()->log(lv,__FILE__,__LINE__,__FUNCTION__,tag,msg,##__VA_ARGS__); } while(0)
#define CXDUMP(lv,tag,buf,sz,msg,...) do{ cx::log::core::instance()->dump(lv,__FILE__,__LINE__,__FUNCTION__,tag,buf,sz,msg,##__VA_ARGS__); } while(0)
#define CXLOG_D(tag,msg,...) CXLOG(cx::log::level::debug,tag,msg,##__VA_ARGS__)
#define CXLOG_I(tag,msg,...) CXLOG(cx::log::level::info,tag,msg,##__VA_ARGS__)
#define CXLOG_W(tag,msg,...) CXLOG(cx::log::level::warn,tag,msg,##__VA_ARGS__)
#define CXLOG_E(tag,msg,...) CXLOG(cx::log::level::error,tag,msg,##__VA_ARGS__)
#define CXLOG_F(tag,msg,...) CXLOG(cx::log::level::fatal,tag,msg,##__VA_ARGS__)
#endif // CX_PLATFORM == CX_P_WINDOWS
#endif // CXLOG

namespace cx::log {

	enum class level { debug, info, warn, error, fatal };

	struct record {
		cx::log::level level;
		const char* file;
		int line;
		const char* function;
		std::chrono::system_clock::time_point time;
		std::thread::id tid;
		const char* tag;
		cx::basic_buffer<char> message;

		record(cx::log::level lv,
			const char *afile,
			const int aline,
			const char *afunction,
			const char *atag)
			: level(lv),
			file(afile),
			line(aline),
			function(afunction),
			tag(atag),
			message(256)
		{
			time = std::chrono::system_clock::now();
			tid = std::this_thread::get_id();
		}
		/*
				enum class type : uint8_t {
					level = 0,
					file,
					line,
					function,
					time,
					process_id,
					thread_id,
					tag,
					message,
					end,
				};

				record(cx::log::level lv,
					   const char *file,
					   const int line,
					   const char *function,
					   const char *tag,
					   void *buf,
					   const std::size_t sz)
					: buf(512)
				{
					uint8_t header[end+1] = {level, file, line, function, time, process_id, thread_id, tag, message , 0xff };
					buf.write(header, end+1);
				}
				cx::basic_buffer<uint8_t> buf;*/
	};
}

#endif