/**
 * @brief
 *
 * @file basic_socket.hpp
 * @author ghtak
 * @date 2018-08-27
 */
#ifndef __cx_io_ip_basic_socket_h__
#define __cx_io_ip_basic_socket_h__

#include <cx/base/defines.hpp>

#include <cx/io/descriptor.hpp>


namespace cx::io::ip {

	template <typename ServiceT>
	class basic_socket : 
		public cx::noncopyable,
		public std::enable_shared_from_this<basic_socket>
	{
	public:
		basic_socket(cx::io::engine& e)
			: _engine(e), _fd(std::make_shared<cx::io::descriptor>()) {

		}

		basic_socket(void) {

		}

	private:
		cx::io::engine& _engine;
		cx::io::descriptor_t _fd;
	};

}

#endif