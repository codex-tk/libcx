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
#include <cx/io/ip/basic_address.hpp>
#include <cx/base/basic_buffer.hpp>

namespace cx::io::ip {

	template < typename EngineType, typename ServiceType> class basic_socket {
	public:
		using engine_type = EngineType;
		using service_type = ServiceType;

		using mux_type = typename service_type::mux_type;
		using descriptor_type = typename service_type::descriptor_type;
		using basic_service_type = typename service_type::basic_service_type;
		using address_type = typename service_type::address_type;

		basic_socket(engine_type& e)
			: _engine(e), _descriptor(std::make_shared<
				typename descriptor_type::element_type>()) {
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
			return basic_service_type::open(_engine, _descriptor, addr, ec);
		}

		void close(void) {
			basic_service_type::close(_engine, _descriptor);
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


		int sendto(const cx::basic_buffer<uint8_t>& buf, const address_type& addr) {
			std::error_code ec;
			return sendto(buf, addr, ec);
		}

		int sendto(const cx::basic_buffer<uint8_t>& buf, const address_type& addr, std::error_code& ec) {
			return basic_service_type::sendto(_descriptor, 
				buf.rdptr(), buf.rdsize(), 0, 
				addr.sockaddr(), addr.length(),
				ec);
		}

		int recvfrom(cx::basic_buffer<uint8_t>& buf, address_type& addr) {
			std::error_code ec;
			return recvfrom(buf, addr, ec);
		}

		int recvfrom(cx::basic_buffer<uint8_t>& buf, address_type& addr, std::error_code& ec) {
			return basic_service_type::recvfrom(_descriptor,
				buf.wrptr(), buf.wrsize(), 0,
				addr.sockaddr(), addr.length_ptr(),
				ec);
		}

	private:
		engine_type& _engine;
		descriptor_type _descriptor;
	};

}

#endif