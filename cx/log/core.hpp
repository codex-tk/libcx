/**
 */
#ifndef __cx_log_core_h__
#define __cx_log_core_h__

#include <cx/cxdefine.hpp>

#include <cx/log/log.hpp>
#include <cx/log/sink.hpp>
#include <cx/log/internal/cout_writer.hpp>
#include <cx/log/internal/string_formatter.hpp>

#include <stdarg.h>

namespace cx::log {

	class core {
	public:
		void log(cx::log::level lv,
			const char* file,
			const int line,
			const char* function,
			const char* tag,
			const char* msg, ...)
		{
			cx::log::record r(lv, file, line, function, tag);
			va_list args;
			va_start(args, msg);
			int len = vsnprintf(r.message.wrptr(), r.message.wrsize(), msg, args);
			va_end(args);
			if (len > r.message.wrsize()) {
				r.message.reserve(len + 1);
				va_list args0;
				va_start(args0, msg);
				len = vsnprintf(r.message.wrptr(), r.message.wrsize(), msg, args0);
				va_end(args0);
			}
			r.message.wrptr(len);
			std::scoped_lock<std::mutex> guard(_lock);
			std::for_each(_sinks.begin(), _sinks.end(), [&](std::shared_ptr<cx::log::sink>& s) {
				s->put(r);
			});
		}

		void dump(cx::log::level lv,
			const char* file,
			const int line,
			const char* function,
			const char* tag,
			void* buf,
			const std::size_t sz,
			const char* msg, ...)
		{
			cx::log::record r(lv, file, line, function, tag);
			va_list args;
			va_start(args, msg);
			int len = vsnprintf(r.message.wrptr(), r.message.wrsize(), msg, args);
			va_end(args);
			if (len > r.message.wrsize()) {
				r.message.reserve(len + 1);
				va_list args0;
				va_start(args0, msg);
				len = vsnprintf(r.message.wrptr(), r.message.wrsize(), msg, args0);
				va_end(args0);
			}
			r.message.wrptr(len);
			char map[] = "0123456789ABCDEF";
			char space = ' ';
			char cr = '\n';
			char end = 0;
			r.message.reserve(sz * 3 + 3);
			r.message.write(&cr, 1);
			char* buf_ptr = static_cast<char*>(buf);
			for (std::size_t i = 0; i < sz; ++i) {
				r.message.write(&map[buf_ptr[i] / 8], 1);
				r.message.write(&map[buf_ptr[i] % 8], 1);
				r.message.write((i + 1) % 16 ? &space : &cr, 1);
			}
			if (sz % 16)
				r.message.write(&cr, 1);
			r.message.write(&end, 1);
			std::scoped_lock<std::mutex> guard(_lock);
			std::for_each(_sinks.begin(), _sinks.end(), [&](std::shared_ptr<cx::log::sink>& s) {
				s->put(r);
			});
		}


		void add_sink(const std::shared_ptr<cx::log::sink>& sink) {
			std::scoped_lock<std::mutex> guard(_lock);
			_sinks.push_back(sink);
		}
		static std::shared_ptr<core> instance(void) {
			static std::shared_ptr<core> core_ptr = std::make_shared<core>();
			return core_ptr;
		}
	private:
		std::mutex _lock;
		std::vector< std::shared_ptr<cx::log::sink>> _sinks;
	};

}

#endif