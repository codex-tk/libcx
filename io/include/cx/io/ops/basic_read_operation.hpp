/**
 * @brief
 *
 * @file basic_read_operation.hpp
 * @author ghtak
 * @date 2018-09-08
 */
#ifndef __cx_io_basic_read_operation_h__
#define __cx_io_basic_read_operation_h__

#include <cx/base/defines.hpp>

namespace cx::io {

	/**
	 * @brief 
	 * 
	 * @tparam ServiceType 
	 */
	template <typename ServiceType, typename base_operation>
	class basic_read_operation : public base_operation {
	public:
		using service_type = ServiceType;
		using descriptor_type = typename service_type::descriptor_type;

		basic_read_operation(void) {}

		virtual ~basic_read_operation(void) {}

		virtual bool complete(const descriptor_type& descriptor) override {
			return service_type::read_complete(descriptor, this);
		}

		virtual void request(const descriptor_type& descriptor) override {
			return service_type::read_request(descriptor, this);
		}
	private:

	};

}

#endif