/**
 * @brief 
 * 
 * @file basic_connect_operation.hpp
 * @author ghtak
 * @date 2018-09-11
 */
#ifndef __cx_io_ip_basic_connect_operation_h__
#define __cx_io_ip_basic_connect_operation_h__

#include <cx/base/defines.hpp>

namespace cx::io::ip {

	/**
	 * @brief 
	 * 
	 * @tparam ServiceType 
	 */
	template <typename HandlerType, typename ServiceType, typename BaseOperation>
	class basic_connect_operation : public BaseOperation {
	public:
		using base_type = BaseOperation;
		using service_type = ServiceType;
		using descriptor_type = typename service_type::descriptor_type;
		using address_type = typename service_type::address_type;

		basic_connect_operation(const address_type& addr, HandlerType&& handler)
			: _address(addr), _handler(std::forward<HandlerType>(handler)) {}

		virtual ~basic_connect_operation(void) {}

		virtual bool complete(const descriptor_type& descriptor) override {
			return service_type::connect_complete(descriptor, this);
		}
        
		address_type& address(void) { return _address; }

		virtual void operator()(void) override {
			HandlerType handler(std::move(_handler));
			std::error_code ec(this->error());
			address_type addr(this->address());;
			delete this;
			handler(ec, addr);
		}
	private:
        address_type _address;
		HandlerType _handler;
	};

}

#endif