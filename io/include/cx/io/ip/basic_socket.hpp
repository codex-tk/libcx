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

namespace cx::io::ip {

	template <
		typename EngineType,
		template < typename T > class ServiceType
	> class basic_socket {
	public:
		using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using descriptor_type = typename mux_type::descriptor_type;

		using service_type = ServiceType<engine_type>;
		using basic_service_type = typename service_type::basic_service_type;

		basic_socket(engine_type& e)
			: _engine(e), _descriptor(std::make_shared<
				typename descriptor_type::element_type>()) {

		}

		~basic_socket(void) {

		}

		template < int Type, int Proto >
		bool open(const cx::io::ip::basic_address<Type, Proto>& addr) {
			std::error_code ec;
			return open(addr, ec);
		}

		template < int Type, int Proto >
		bool open(const cx::io::ip::basic_address<Type, Proto>& addr,
			std::error_code& ec)
		{
			return basic_service_type::open(_descriptor, addr, ec);
		}

	private:
		engine_type& _engine;
		descriptor_type _descriptor;
	};

}

#endif