/**
 */
#ifndef __cx_log_cout_writer_h__
#define __cx_log_cout_writer_h__

#include <cx/log/writer.hpp>

namespace cx::log {

	class cout_writer : public cx::log::writer {
	public:
		enum code {
			black = 30,
			red = 31,
			green = 32,
			yellow = 33,
			blue = 34,
			magenta = 35,
			cyan = 36,
			white = 37,
		};
		virtual void operator()(const cx::log::record& r, const cx::basic_buffer<char>& formatted) {
#if defined(CX_PLATFORM_WIN32)   
			CX_UNUSED(r);
			std::cout << formatted.rdptr() << std::endl;
#else
			code color_code;
			switch (r.level) {
			case cx::log::level::debug: color_code = code::cyan; break;
			case cx::log::level::info: color_code = code::green; break;
			case cx::log::level::warn: color_code = code::magenta; break;
			case cx::log::level::error: color_code = code::red; break;
			case cx::log::level::fatal: color_code = code::yellow; break;
			}
			std::cout << "\033[" << color_code << "m" << formatted.rdptr() << std::endl;
#endif
		}
	public:
		static std::shared_ptr<writer> instance(void) {
			static std::shared_ptr<writer> writer_ptr = std::make_shared<cout_writer>();
			return writer_ptr;
		}
	};
}

#endif