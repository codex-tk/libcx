/**
*/
#ifndef __cx_log_filter_h__
#define __cx_log_filter_h__

#include <cx/log/log.hpp>

namespace cx::log {
	class filter {
	public:
		filter(void) {}
		virtual ~filter(void) {}
		virtual bool operator()(const cx::log::record&) {
			return true;
		}
	public:
		static std::shared_ptr<filter> default_filter(void) {
			static std::shared_ptr<filter> filter_ptr = std::make_shared<filter>();
			return filter_ptr;
		}
	};
}

#endif