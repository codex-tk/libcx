/**
 * @brief
 *
 * @file basic_socket.hpp
 * @author ghtak
 * @date 2018-05-22
 */

#ifndef __cx_io_ip_basic_socket_h__
#define __cx_io_ip_basic_socket_h__

#include <cx/io/ip/option.hpp>
#include <cx/io/basic_object.hpp>

namespace cx::io::ip {

	template < typename ServiceType >
	class basic_socket : public cx::io::basic_object<ServiceType> {
	public:
		using cx::io::basic_object<ServiceType>::write;
		using cx::io::basic_object<ServiceType>::read;

		template < typename EngineType >
		basic_socket(EngineType& engine)
			: basic_object(engine)
		{
		}

		basic_socket(ServiceType& service, handle_type handle)
			: basic_object(service, handle)
		{
		}

		bool connect(const address_type& addr) {
			return service().connect(handle(), addr);
		}

		bool connect(const address_type& addr, const std::chrono::milliseconds& ms) {
			if (connect(addr)) {
				if (cx::io::ops::write == service().poll(handle(), cx::io::ops::write, ms))
					return true;
			}
			return false;
		}

		int write(const buffer_type& buf, const std::chrono::milliseconds& ms) {
			if (cx::io::ops::write == service().poll(handle(), cx::io::ops::write, ms))
				return this->write(buf);
			return -1;
		}

		int read(buffer_type& buf, const std::chrono::milliseconds& ms) {
			if (cx::io::ops::read == service().poll(handle(), cx::io::ops::read, ms))
				return this->read(buf);
			return -1;
		}

		bool bind(const address_type& addr) {
			return service().bind(handle(), addr);
		}

		int shutdown(int how) {
			return service().shutdown(handle(), how);
		}

		address_type local_address(void) const {
			return service().local_address(handle());
		}

		address_type remote_address(void) const {
			return service().remote_address(handle());
		}

		template < typename T >
		bool set_option(T&& opt) {
			return service().set_option(handle(), std::forward<T>(opt));
		}

		template < typename T >
		bool get_option(T&& opt) {
			return service().get_option(handle(), std::forward<T>(opt));
		}
	};

} // cx::io::ip

#endif
