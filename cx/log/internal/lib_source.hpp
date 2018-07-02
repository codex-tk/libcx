/**
 */
#ifndef __cx_log_lib_source_h__
#define __cx_log_lib_source_h__

#include <cx/log/source.hpp>

namespace cx::log {

	class lib_source : public basic_source<lib_source> {
	public:
		lib_source(void)
			: _name("cx") {}
		virtual const char* name(void) {
			return _name.c_str();
		}
		static source* instance(void) {
			static lib_source src;
			return &src;
		}
	private:
		std::string _name;
	};

}

#endif