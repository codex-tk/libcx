/**
 */
#ifndef __cx_io_ip_detail_basic_accept_op_h__
#define __cx_io_ip_detail_basic_accept_op_h__

#include <cx/io/ip/basic_socket.hpp>

namespace cx::io::ip{

    template < typename ServiceType >
	class basic_accept_op : public ServiceType::operation_type {
	public:
		using address_type = typename ServiceType::address_type;

		basic_accept_op(const cx::io::ip::basic_socket<ServiceType>& fd)
			: _socket(fd)
		{}

		cx::io::ip::basic_socket<ServiceType>& socket(void) {
			return _socket;
		}
		address_type& address(void) {
			return _address;
		}
	private:
		address_type _address;
		cx::io::ip::basic_socket<ServiceType> _socket;
	};

}

#endif