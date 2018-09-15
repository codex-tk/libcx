/**
 * @brief 
 * 
 * @file basic_accept_operation.hpp
 * @author ghtak
 * @date 2018-09-13
 */
#ifndef __cx_io_ip_basic_accept_operation_h__
#define __cx_io_ip_basic_accept_operation_h__

#include <cx/base/defines.hpp>

namespace cx::io::ip {

	/**
	 * @brief 
	 * 
	 * @tparam ServiceType 
	 */
	template <typename HandlerType, typename ServiceType, typename BaseOperation>
	class basic_accept_operation : public BaseOperation {
	public:
		using base_type = BaseOperation;
		using service_type = ServiceType;
		using descriptor_type = typename service_type::descriptor_type;
		using address_type = typename service_type::address_type;

		basic_accept_operation(const descriptor_type& accepted, HandlerType&& handler)
			: _accepted(accepted), _handler(std::forward<HandlerType>(handler)) {}

		virtual ~basic_accept_operation(void) {}

		virtual bool complete(const descriptor_type& descriptor) override {
			return service_type::accept_complete(descriptor, this);
		}

		const descriptor_type& accepted(void) { return _accepted; }
		
		address_type& address(void) { return _address; } 

		virtual void operator()(void) override {
			HandlerType handler(std::move(_handler));
			std::error_code ec(this->error());
			address_type addr(this->address());
			delete this;
			handler(ec, addr);
		}
	private:
		const descriptor_type& _accepted;
		address_type _address;
		HandlerType _handler;
	};

}

#endif