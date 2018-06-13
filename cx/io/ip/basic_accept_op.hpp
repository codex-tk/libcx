/**
 */
#ifndef __cx_io_ip_detail_basic_accept_op_h__
#define __cx_io_ip_detail_basic_accept_op_h__

#include <cx/io/ip/basic_accept_context.hpp>

namespace cx::io::ip{

    template < typename ServiceType >
	class basic_accept_op : public ServiceType::operation_type {
	public:
		using address_type = typename ServiceType::address_type;

		basic_accept_op(ServiceType& svc)
			: _accept_context(svc) {}

		cx::io::ip::basic_accept_context<ServiceType>& accept_context(void) {
			return _accept_context;
		}

		address_type& address(void) {
			return _address;
		}
	private:
		address_type _address;
		cx::io::ip::basic_accept_context<ServiceType> _accept_context;
	};

}

#endif