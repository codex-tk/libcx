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
			_address = addr;
			if (_fd.open(addr)) {
				if (_fd.set_option(cx::io::ip::option::reuse_address())) {
					if (_fd.bind(addr)) {
						if (_fd.service().listen(_fd.handle(), SOMAXCONN)) {
#if CX_PLATFORM == CX_P_WINDOWS
							_fd.service().implementation().bind(_fd.handle(), 0);
#endif
							return true;
						}
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

		cx::io::ip::basic_socket<ServiceType> accept(address_type& addr
			, const std::chrono::milliseconds& ms) {
			if (cx::io::ops::read == _fd.service().poll(_fd.handle(), cx::io::ops::read, ms)) {
				handle_type handle = _fd.service().accept(_fd.handle(), addr);
				return cx::io::ip::basic_socket<ServiceType>(_fd.service(), handle);
			}
			return cx::io::ip::basic_socket<ServiceType>(_fd.service()
				, _fd.service().make_shared_handle());
		}

		template < typename HandlerType >
		void async_accept(HandlerType&& handler) {
			_fd.async_accept(_address,std::forward<HandlerType>(handler));
		}

		handle_type handle(void) {
			return _fd.handle();
		}
	private:
		cx::io::ip::basic_socket<ServiceType> _fd;
		address_type _address;
	};

}

#endif