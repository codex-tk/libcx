/**
 */
#ifndef __cx_log_writer_h__
#define __cx_log_writer_h__

#include <cx/log/log.hpp>

namespace cx::log {
	class writer {
	public:
		writer(void) = default;
		virtual ~writer(void) = default;
		virtual void operator()(const cx::log::record& record,
			const cx::basic_buffer<char>& formatted) = 0;
	};
}

#endif