/**
 * @brief 
 * 
 * @file basic_timer.hpp
 * @author ghtak
 * @date 2018-09-17
 */

#ifndef __cx_timer_basic_timer_h__
#define __cx_timer_basic_timer_h__

#include <cx/base/defines.hpp>
#include <cx/base/noncopyable.hpp>

#include <cx/io/ops/handler_operation.hpp>

namespace cx::timer {

	template <typename EngineType> class basic_timer
		: private cx::noncopyable {
	public:
		using engine_type = EngineType;
		using operation_type = typename engine_type::operation_type;
		using service_type = typename engine_type::timer_service_type;
		using descriptor_type = typename service_type::descriptor_type;

		basic_timer(engine_type& e)
			: _descriptor(std::make_shared<
				typename descriptor_type::element_type>(e)) {
		}

		~basic_timer(void) {}

		template <typename HandlerType>
		void schedule(const std::chrono::system_clock::time_point& expired_at, HandlerType&& handler) {
			class timer_handler_operation : public operation_type {
			public:
				timer_handler_operation(HandlerType&& handler)
					: _handler(std::forward<HandlerType>(handler)) {}

				virtual void operator()(void) override {
					HandlerType handler(std::move(_handler));
					std::error_code ec(this->error());
					delete this;
					handler(ec);
				}
			private:
				HandlerType _handler;
			};
			if (_descriptor->op) {
				assert(0);
				_descriptor->engine.cancel(_descriptor);
			}
			_descriptor->expired_at = expired_at;
			_descriptor->op = new timer_handler_operation(std::forward<HandlerType>(handler));
			_descriptor->engine.schedule(_descriptor);
		}

		void cancel(void) {
			_descriptor->engine.cancel(_descriptor);
		}
	private:
		descriptor_type _descriptor;
	};
}

#endif