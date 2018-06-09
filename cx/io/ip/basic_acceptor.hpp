/**
*/
#ifndef __cx_io_ip_basic_acceptor_h__
#define __cx_io_ip_basic_acceptor_h__

#include <cx/io/ip/basic_socket.hpp>

namespace cx::io::ip {

	template < typename ServiceType >
	class basic_acceptor {
	public:
		using address_type = typename ServiceType::address_type;
		using handle_type = typename ServiceType::handle_type;
		template < typename EngineType >
		basic_acceptor(EngineType& engine)
			: _fd(engine)
		{
		}

		bool open(const address_type& addr) {
			if (_fd.open(addr)) {
				if (_fd.set_option(cx::io::ip::option::reuse_address())) {
					if (_fd.bind(addr)) {
						return _fd.service().listen(_fd.handle(), SOMAXCONN);
					}
				}
			}
			return false;
		}

		template < typename T >
		bool set_option(T&& t) {
			return _fd.set_option(std::forward<T>(t));
		}

		void close(void) {
			_fd.close();
		}

		cx::io::ip::basic_socket<ServiceType> accept(address_type& addr) {
			handle_type handle = _fd.service().accept(_fd.handle(), addr);
			return cx::io::ip::basic_socket<ServiceType>(_fd.service(), handle);
		}
	private:
		cx::io::ip::basic_socket<ServiceType> _fd;
	};

}

#endif