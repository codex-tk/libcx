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
#include <cx/io/buffer.hpp>

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

		cx::io::buffer& buffer(void) {
			return _buffer;
		}
	private:
		cx::io::buffer _buffer;

	};

	/**
	* @brief
	*
	* @tparam ServiceType
	*/
	template <typename ServiceType, typename base_operation>
	class writen_operation : public basic_write_operation<ServiceType, base_operation> {
	public:
		using base_type = base_operation;
		using service_type = ServiceType;
		using descriptor_type = typename service_type::descriptor_type;
		using address_type = typename service_type::address_type;

		writen_operation(void) : _total_write_size(0) {}

		virtual ~writen_operation(void) {}

		virtual bool complete(const descriptor_type& descriptor) override {
			return service_type::write_complete(descriptor, this);
		}

		virtual void request(const descriptor_type& descriptor) override {
			return service_type::write_request(descriptor, this);
		}

		void consume(int n) {
			n = std::min(static_cast<decltype(this->buffer().length())>(n),
				this->buffer().length());
			_total_write_size += n;
			this->buffer().reset(
				static_cast<int8_t*>(this->buffer().base()) + n,
				this->buffer().length() - n
			);
		}

		int total_write_size(void) {
			return _total_write_size;
		}
	private:
		int _total_write_size;
	};
}

#endif