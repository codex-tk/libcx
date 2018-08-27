/**
 * @brief 
 * 
 * @file epoll.hpp
 * @author ghtak
 * @date 2018-08-26
 */

#ifndef __cx_io_completion_port_h__
#define __cx_io_completion_port_h__

#include <cx/base/defines.hpp>
#include <cx/io/descriptor.hpp>

#if defined(CX_PLATFORM_LINUX)

namespace cx::io::internal {

	/**
	* @brief
	*
	*/
	class epoll
		: public cx::noncopyable {
	public:

		epoll(cx::io::engine& e);

		~epoll(void);

		bool bind(const cx::io::descriptor_t& fd, int ops);

		bool bind(const cx::io::descriptor_t& fd, int ops, std::error_code& ec);

		void unbind(const cx::io::descriptor_t& fd);

		void wakeup(void);

		int run(const std::chrono::milliseconds& wait_ms);

	private:
		cx::io::engine& _engine;
		int _handle;
		int _eventfd;
	};

	using impl_t = epoll;
}

#endif

#endif