/**
 */
#ifndef __cx_io_ip_detail_basic_accept_op_h__
#define __cx_io_ip_detail_basic_accept_op_h__

namespace cx::io::ip{

    template < typename ServiceType >
	class basic_accept_op : public ServiceType::operation_type {
	public:
		using address_type = typename ServiceType::address_type;
		using native_handle_type = typename ServiceType::native_handle_type;

		basic_accept_op(void) noexcept 
			: _raw_handle(ServiceType::invalid_native_handle) {}

		native_handle_type raw_handle(void) {
			return _raw_handle;
		}

		void raw_handle(native_handle_type raw_handle){
			_raw_handle = raw_handle;
		}

		address_type& address(void) {
			return _address;
		}
	private:
		address_type _address;
		native_handle_type _raw_handle;
	};

}

#endif