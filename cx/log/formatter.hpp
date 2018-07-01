/**
*/
#ifndef __cx_log_formatter_h__
#define __cx_log_formatter_h__

#include <cx/cxdefine.hpp>

namespace cx::log {

	class formatter {
	public:
		formatter(void) = default;
		virtual ~formatter(void) = default;

		virtual void operator()(const cx::log::record& record,
			cx::basic_buffer<char>& buf) = 0;
	};

}

#endif