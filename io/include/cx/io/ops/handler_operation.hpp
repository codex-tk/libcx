/**
 * @brief 
 * 
 * @file handler_operation.hpp
 * @author ghtak
 * @date 2018-09-09
 */


#ifndef __cx_io_handler_operation_h__
#define __cx_io_handler_operation_h__

#include <cx/base/defines.hpp>
#include <cx/base/basic_buffer.hpp>
#include <cx/io/ops/basic_dgram_operation.hpp>

namespace cx::io {

	/**
	* @brief
	*
	* @tparam ServiceType
	*/
	template <typename BaseType, typename HandlerType>
	class handler_operation : public BaseType {
	public:
		template < typename T >
		handler_operation(const T& t, HandlerType&& handler)
			: BaseType(t)
			, _handler(std::forward<HandlerType>(handler)) {}

		handler_operation(HandlerType&& handler)
			: _handler(std::forward<HandlerType>(handler)) {}

		virtual void operator()(void) override {
			HandlerType handler(std::move(_handler));
			std::error_code ec(this->error());
			int size(this->size());
			delete this;
			handler(ec, size);
		}
	private:
		HandlerType _handler;
	};

	/**
	* @brief
	*
	* @tparam ServiceType
	*/
	template <typename BaseType, typename HandlerType>
	class dgram_handler_operation : public BaseType {
	public:
		using address_type = typename BaseType::address_type;
		template < typename T >
		dgram_handler_operation(const T& t, HandlerType&& handler)
			: BaseType(t)
			, _handler(std::forward<HandlerType>(handler)) {}

		dgram_handler_operation(HandlerType&& handler)
			: _handler(std::forward<HandlerType>(handler)) {}

		virtual void operator()(void) override {
			HandlerType handler(std::move(_handler));
			std::error_code ec(this->error());
			int size(this->size());
			address_type addr(this->address());
			delete this;
			handler(ec, size, addr);
		}
	private:
		HandlerType _handler;
	};

}

#endif