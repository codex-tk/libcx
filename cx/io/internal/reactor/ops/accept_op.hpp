
/**
 * */
#ifndef __cx_io_internal_reactor_ip_accept_op_h__
#define __cx_io_internal_reactor_ip_accept_op_h__

#include <cx/io/ip/basic_accept_op.hpp>

namespace cx::io::internal::reactor::ip {

	template < typename ServiceType, typename HandlerType >
	class accept_op : public cx::io::ip::basic_accept_op< ServiceType > {
	public:
		using handle_type = typename ServiceType::handle_type;
		using implementation_type = typename ServiceType::implementation_type;
		
		accept_op(ServiceType& svc, HandlerType&& handler)
			: cx::io::ip::basic_accept_op< ServiceType >(svc)
			, _handler(std::forward<HandlerType>(handler)) {}

		virtual ~accept_op(void) {}

		virtual bool complete(const typename basic_reactor<implementation_type>::handle_type& handle) override {
			handle_type ptr = std::static_pointer_cast<typename handle_type::element_type>(handle);
			return ptr->service.accept_complete(ptr, this);
		}

		virtual int operator()(void) override {
			_handler(this->error(), this->accept_context(), this->address());
			delete this;
			return 1;
		}
	private:
		HandlerType _handler;
	};

}

#endif
