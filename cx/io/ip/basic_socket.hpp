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
        using implementation_type = typename ServiceType::implementation_type;
        using handle_type = typename ServiceType::handle_type;
		using native_handle_type = typename ServiceType::native_handle_type;
        using address_type = typename ServiceType::address_type;
        using buffer_type = typename ServiceType::buffer_type;

        using base_type = cx::io::basic_object<ServiceType>; 
        using base_type::write;
        using base_type::read;
        using base_type::handle;
        using base_type::service;

        template < typename EngineType >
		basic_socket(EngineType& engine)
			: base_type(engine) {}

		basic_socket(ServiceType& service, handle_type handle)
			: base_type(service, handle) {}

	    template < typename EngineType >
		basic_socket(EngineType& e, native_handle_type raw_handle)
			: base_type(e.template service<ServiceType>()
				, e.template service<ServiceType>().make_shared_handle(raw_handle)) {}

		basic_socket(const basic_socket& rhs)
			: base_type(rhs.service(), rhs.handle()) {}

		bool connect(const address_type& addr) {
			return service().connect(handle(), addr);
		}

		bool connect(const address_type& addr, const std::chrono::milliseconds& ms) {
			if (connect(addr)) {
				if (cx::io::pollout == service().poll(handle(), cx::io::pollout, ms))
					return true;
			}
			return false;
		}

		int write(const buffer_type& buf, const std::chrono::milliseconds& ms) {
			if (cx::io::pollout == service().poll(handle(), cx::io::pollout, ms))
				return this->write(buf);
			return -1;
		}

		int read(buffer_type& buf, const std::chrono::milliseconds& ms) {
			if (cx::io::pollin == service().poll(handle(), cx::io::pollin, ms))
				return this->read(buf);
			return -1;
		}

		bool bind(const address_type& addr) {
			return service().bind(handle(), addr);
		}

		bool shutdown(int how) {
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
	public:
		template < typename HandlerType >
		void async_connect(const address_type& addr, HandlerType&& handler) {
			service().async_connect(handle(), addr, std::forward<HandlerType>(handler));
		}
		template < typename HandlerType >
		void async_accept( HandlerType&& handler) {
			service().async_accept(handle(), std::forward<HandlerType>(handler));
		}
	};

} // cx::io::ip

#endif
