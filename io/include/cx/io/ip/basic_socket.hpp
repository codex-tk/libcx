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
#include <cx/base/basic_buffer.hpp>
#include <cx/io/ip/ip.hpp>
#include <cx/io/ip/basic_address.hpp>

namespace cx::io::ip {

	template < typename EngineType, typename ServiceType> class basic_socket {
	public:
		using engine_type = EngineType;
		using service_type = ServiceType;

		using mux_type = typename service_type::mux_type;
		using descriptor_type = typename service_type::descriptor_type;
		using basic_service_type = typename service_type::basic_service_type;
		using address_type = typename service_type::address_type;
		using buffer_type = typename service_type::buffer_type;

		basic_socket(engine_type& e)
			: _descriptor(std::make_shared<
				typename descriptor_type::element_type>(e)) {
		}

		~basic_socket(void) {

		}

		bool open(const address_type& addr) {
			std::error_code ec;
			return open(addr, ec);
		}

		bool open(const address_type& addr,
			std::error_code& ec)
		{
			return basic_service_type::open(_descriptor, addr, ec);
		}

		void close(void) {
			basic_service_type::close(_descriptor);
		}

		bool good(void) {
			return mux_type::good(_descriptor);
		}

		template < typename T >
		bool set_option(T&& opt) {
			return opt.set(mux_type::socket_handle(_descriptor));
		}

		template < typename T >
		bool get_option(T&& opt) {
			return opt.get(mux_type::socket_handle(_descriptor));
		}

		bool bind(const address_type& addr) {
			std::error_code ec;
			return bind(addr, ec);
		}

		bool bind(const address_type& addr, std::error_code& ec) {
			return basic_service_type::bind(_descriptor, addr, ec);
		}

		template <typename = std::enable_if_t<is_dgram_available<ServiceType>::value> >
		int sendto(const buffer_type& buf, const address_type& addr) {
			std::error_code ec;
			return sendto(buf, addr, ec);
		}

		template <typename = std::enable_if_t<is_dgram_available<ServiceType>::value> >
		int sendto(const buffer_type& buf, const address_type& addr, std::error_code& ec) {
			return basic_service_type::sendto(_descriptor, 
				buf.base(), buf.length(), 0,
				addr.sockaddr(), addr.length(),
				ec);
		}

		template <typename = std::enable_if_t<is_dgram_available<ServiceType>::value> >
		int recvfrom(const buffer_type& buf, address_type& addr) {
			std::error_code ec;
			return recvfrom(buf, addr, ec);
		}

		template <typename = std::enable_if_t<is_dgram_available<ServiceType>::value> >
		int recvfrom(const buffer_type& buf, address_type& addr, std::error_code& ec) {
			return basic_service_type::recvfrom(_descriptor,
				buf.base(), buf.length(), 0,
				addr.sockaddr(), addr.length_ptr(),
				ec);
		}

		template <typename = std::enable_if_t<is_stream_available<ServiceType>::value> >
		int send(const buffer_type& buf) {
			std::error_code ec;
			return send(buf, ec);
		}

		template <typename = std::enable_if_t<is_stream_available<ServiceType>::value> >
		int send(const buffer_type& buf, std::error_code& ec) {
			return basic_service_type::send(_descriptor,
				buf.base(), buf.length(), 0, ec);
		}

		template <typename = std::enable_if_t<is_stream_available<ServiceType>::value> >
		int recv(buffer_type& buf) {
			std::error_code ec;
			return recv(buf, ec);
		}

		template <typename = std::enable_if_t<is_stream_available<ServiceType>::value> >
		int recv(buffer_type& buf, std::error_code& ec) {
			return basic_service_type::recv(_descriptor,
				buf.base(), buf.length(), 0, ec);
		}

		template <typename HandlerType, typename = std::enable_if_t<is_dgram_available<ServiceType>::value> >
		void async_send(const buffer_type& buf, const address_type& addr, HandlerType&& handler) {
			service_type::write(_descriptor, buf, addr, std::forward<HandlerType>(handler));
		}

		template <typename HandlerType, typename = std::enable_if_t<is_dgram_available<ServiceType>::value> >
		void async_recv(buffer_type& buf, address_type& addr, HandlerType&& handler) {
			service_type::read(_descriptor, buf, addr, std::forward<HandlerType>(handler));
		}

		template <typename HandlerType, typename = std::enable_if_t<is_stream_available<ServiceType>::value> >
		void async_send(buffer_type& buf, HandlerType&& handler) {
			service_type::write(_descriptor, buf, std::forward<HandlerType>(handler));
		}

		template <typename HandlerType, typename = std::enable_if_t<is_stream_available<ServiceType>::value> >
		void async_recv(buffer_type& buf, HandlerType&& handler) {
			service_type::read(_descriptor, buf, std::forward<HandlerType>(handler));
		}

		descriptor_type& descriptor(void) {
			return _descriptor;
		}

		int poll(int ops, const std::chrono::milliseconds& ms) {
			std::error_code ec;
			return basic_service_type::poll(_descriptor, ops, ms, ec);
		}

		bool connect(const address_type& addr) {
			std::error_code ec;
			return connect(addr, ec);
		}

		bool connect(const address_type& addr, std::error_code& ec) {
			return basic_service_type::connect(_descriptor, addr.sockaddr(), addr.length(), ec);
		}
	private:
		descriptor_type _descriptor;
	};

}

#endif