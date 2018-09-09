/**
 * @brief 
 * 
 * @file basic_dgram_operation.hpp
 * @author ghtak
 * @date 2018-09-09
 */
#ifndef __cx_io_basic_dgram_operation_h__
#define __cx_io_basic_dgram_operation_h__

#include <cx/base/defines.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ops/basic_io_operation.hpp>

namespace cx::io::ip {

	/**
	* @brief
	*
	* @tparam ServiceType
	*/
	template <typename ServiceType>
	class basic_dgram_operation : public basic_io_operation<ServiceType>{
	public:
		using service_type = ServiceType;
		using descriptor_type = typename service_type::descriptor_type;
		using address_type = typename service_type::address_type;
		
		basic_dgram_operation(void) {}

		virtual ~basic_dgram_operation(void) {}

		address_type& address(void) { return _address; }
	private:
		address_type _address;
	};

}


#endif