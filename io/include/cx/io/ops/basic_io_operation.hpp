/**
 * @brief 
 * 
 * @file basic_io_operation.hpp
 * @author ghtak
 * @date 2018-09-09
 */
#ifndef __cx_io_basic_io_operation_h__
#define __cx_io_basic_io_operation_h__

#include <cx/base/defines.hpp>
#include <cx/base/basic_buffer.hpp>

namespace cx::io {

	/**
	* @brief
	*
	* @tparam ServiceType
	*/
	template <typename ServiceType>
	class basic_io_operation : public ServiceType::operation_type {
	public:
		using service_type = ServiceType;
		using descriptor_type = typename service_type::descriptor_type;

		basic_io_operation(void) {}

		virtual ~basic_io_operation(void) {}

		cx::basic_buffer<uint8_t>& buffer(void) {
			return _buffer;
		}
	private:
		cx::basic_buffer<uint8_t> _buffer;
	};

}


#endif