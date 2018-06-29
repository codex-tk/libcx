/**
 * */
#ifndef __cx_io_ip_read_op_h__
#define __cx_io_ip_read_op_h__

#include <cx/io/basic_read_op.hpp>

namespace cx::io::internal::reactor::ip {

	template < typename ServiceType >
	class read_op : public cx::io::basic_read_op< ServiceType > {
	public:
		using buffer_type = typename ServiceType::buffer_type;
		using handle_type = typename ServiceType::handle_type;
		using implementation_type = typename ServiceType::implementation_type;

		read_op(void) {}

		read_op(const buffer_type& buf)
			: basic_read_op<ServiceType>(buf) {}

		virtual ~read_op(void) {}

		virtual bool complete(const typename basic_reactor<implementation_type>::handle_type& handle) override {
			handle_type ptr = std::static_pointer_cast<typename handle_type::element_type>(handle);
			return ptr->service.read_complete(ptr, this);
		}
	};
}

#endif
