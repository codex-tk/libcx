/**
 * @brief
 *
 * @file engine.hpp
 * @author ghtak
 * @date 2018-08-26
 */

#ifndef __cx_io_basic_engine_h__
#define __cx_io_basic_engine_h__

#include <cx/base/defines.hpp>
#include <cx/base/noncopyable.hpp>

#if defined(CX_PLATFORM_WIN32)
#include <cx/io/mux/completion_port.hpp>
#elif defined(CX_PLATFORM_LINUX)
#include <cx/io/mux/epoll.hpp>
#endif

namespace cx::io {

	/**
	 * @brief
	 *
	 */
	class engine
		: public cx::noncopyable {
	public:

		engine(void);
		
		~engine(void);

		cx::io::mux::impl_t& implementation(void);

	private:
		cx::io::mux::impl_t _impl;
	};

}

#endif