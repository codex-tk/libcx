/**
 * @brief 
 * 
 * @file epoll.hpp
 * @author ghtak
 * @date 2018-08-26
 */

#ifndef __cx_io_epoll_h__
#define __cx_io_epoll_h__

#include <cx/base/defines.hpp>
#include <cx/io/descriptor.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io::internal {

	/**
	* @brief
	*
	*/
	class completion_port {
	public:

		completion_port(cx::io::engine& e);

		~completion_port(void);

		bool bind(const cx::io::descriptor_t& fd);

		bool bind(const cx::io::descriptor_t& fd, int ops);

		void unbind(const cx::io::descriptor_t& fd);

		void wakeup(void);

		int run(const std::chrono::milliseconds& wait_ms);

	private:
		cx::io::engine& _engine;
		HANDLE _handle;
	};

}

#endif

#endif