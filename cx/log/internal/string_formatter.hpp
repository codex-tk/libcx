/**
*/
#ifndef __cx_log_string_formatter_h__
#define __cx_log_string_formatter_h__

#include <cx/log/formatter.hpp>

namespace cx::log {

	class string_formatter : public cx::log::formatter {
	public:
		string_formatter(void) {}
		virtual ~string_formatter(void) {}

		virtual void operator()(const cx::log::record& record, cx::basic_buffer<char>& buf) {
			time_t ts = std::chrono::system_clock::to_time_t(record.time);
			struct tm tm;
#if CX_PLATFORM == CX_P_WINDOWS
			localtime_s(&tm, &ts);
#else
			localtime_r(&ts, &tm);
#endif	
			std::size_t tid = std::hash<std::thread::id>{}(record.tid);

			static char level[] = { 'D','I','W','E','F' };
			int len = snprintf(buf.wrptr(), buf.wrsize(),
				"[%04d%02d%02d %02d%02d%02d][%c][%s][%s][%s][%s:%d][%zu]\r\n",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
				level[static_cast<int>(record.level)],
				record.source->name(),
				record.message.rdptr(),
				record.function,
				record.file,
				record.line,
				tid
			);
			if (len >= buf.wrsize()) {
				buf.reserve(len + 1);
				len = snprintf(buf.wrptr(), len,
					"[%04d%02d%02d %02d%02d%02d][%c][%s][%s][%s][%s:%d][%zu]\r\n",
					tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
					level[static_cast<int>(record.level)],
					record.source->name(),
					record.message.rdptr(),
					record.function,
					record.file,
					record.line,
					tid
				);
			}
			buf.wrptr(len);
		}
	public:
		static std::shared_ptr<formatter> instance(void) {
			static std::shared_ptr<formatter> ptr = std::make_shared<string_formatter>();
			return ptr;
		}
	};

}

#endif