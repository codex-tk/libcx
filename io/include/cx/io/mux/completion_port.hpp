/**
 * @brief 
 * 
 * @file completion_port.hpp
 * @author ghtak
 * @date 2018-08-31
 */
#ifndef __cx_io_completion_port_h__
#define __cx_io_completion_port_h__

#include <cx/base/defines.hpp>
#include <cx/io/descriptor.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io::mux {

	/**
	* @brief
	*
	*/
	class completion_port
		: public cx::noncopyable {
	public:

		completion_port(cx::io::engine& e);

		~completion_port(void);

		bool bind(const cx::io::descriptor_t& fd);

		bool bind(const cx::io::descriptor_t& fd, std::error_code& ec);

		bool bind(const cx::io::descriptor_t& fd, int ops);

		bool bind(const cx::io::descriptor_t& fd, int ops, std::error_code& ec);

		void unbind(const cx::io::descriptor_t& fd);

		void wakeup(void);

		int run(const std::chrono::milliseconds& wait_ms);

	private:
		cx::io::engine& _engine;
		HANDLE _handle;
	};

	using impl_t = completion_port;
}

#endif

#endif