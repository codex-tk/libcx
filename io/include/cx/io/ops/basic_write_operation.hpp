/**
 * @brief
 *
 * @file basic_write_operation.hpp
 * @author ghtak
 * @date 2018-09-08
 */
#ifndef __cx_io_basic_write_operation_h__
#define __cx_io_basic_write_operation_h__

#include <cx/base/defines.hpp>

namespace cx::io {

    /**
     * @brief 
     * 
     * @tparam ServiceType 
     */
	template <typename ServiceType, typename base_operation>
	class basic_write_operation : public base_operation {
	public:
		using base_type = base_operation;
		using service_type = ServiceType;
		using descriptor_type = typename service_type::descriptor_type;
		using address_type = typename service_type::address_type;

		basic_write_operation(void) {}

		virtual ~basic_write_operation(void) {}

		virtual bool complete(const descriptor_type& descriptor) override {
			return ServiceType::write_complete(descriptor, this);
		}

		virtual void request(const descriptor_type& descriptor) override {
			return service_type::write_request(descriptor, this);
		}
	private:

	};

}


#endif