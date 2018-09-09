/**
 * @brief 
 * 
 * @file basic_dgram_service.hpp
 * @author ghtak
 * @date 2018-09-01
 */
#ifndef __cx_io_ip_basic_dgram_service_h__
#define __cx_io_ip_basic_dgram_service_h__

#include <cx/base/defines.hpp>
#include <cx/base/error.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/services/basic_service.hpp>

namespace cx::io::ip {

	template < typename EngineType, int Type = SOCK_DGRAM , int Proto = IPPROTO_UDP > 
	class basic_dgram_service {
	public:
		using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using operation_type = typename mux_type::operation_type;
		using descriptor_type = typename mux_type::descriptor_type;
		using basic_service_type = cx::io::ip::basic_service<engine_type>;
		using address_type = cx::io::ip::basic_address<Type, Proto>;


	};
}
#endif