/**
 * @brief
 *
 * @file basic_timer_service.hpp
 * @author ghtak
 * @date 2018-09-17
 */

#ifndef __cx_timer_basic_timer_service_h__
#define __cx_timer_basic_timer_service_h__

#include <cx/base/defines.hpp>
#include <cx/base/noncopyable.hpp>

#include <cx/stdext/removable_priority_queue.hpp>

namespace cx::timer {

	template <typename EngineType> class basic_timer_service
		: private cx::noncopyable {
	public:
		using engine_type = EngineType;
		using operation_type = typename engine_type::operation_type;
		struct descriptor;
		using descriptor_type = std::shared_ptr<descriptor>;

		struct descriptor :
			private cx::noncopyable,
			public std::enable_shared_from_this<descriptor>
		{
			engine_type& engine;
			std::chrono::system_clock::time_point expired_at;
			operation_type* op;

			descriptor(engine_type& e) : engine(e) , op(nullptr){}
		};

		basic_timer_service(engine_type& e) : _engine(e) {
			_wakeup_after = std::chrono::milliseconds(std::numeric_limits<int>::max());
		}

		~basic_timer_service(void) {}

		engine_type& engine(void) { return _engine; }

		void schedule(const descriptor_type& timer_fd) {
			_timer_queue.push(timer_fd);
			update_wakeup_after();
		}

		std::chrono::milliseconds wakeup_after(void) {
			return _wakeup_after;
		}

		cx::slist<operation_type> drain_expired_timers(void) {
			cx::slist<operation_type> ops;
			auto now = std::chrono::system_clock::now();
			while (!_timer_queue.empty()) {
				auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
					_timer_queue.top()->expired_at - now);
				if (diff.count() < 100) { // 100 ms
					if (_timer_queue.top()->op) {
						operation_type* op = _timer_queue.top()->op;
						op->set(std::make_error_code(std::errc::timed_out), 0);
						ops.add_tail(op);
					}
					_timer_queue.top()->op = nullptr;
					_timer_queue.pop();
				}
				else {
					break;
				}
			}
			update_wakeup_after();
			return ops;
		}

		operation_type* cancel(const descriptor_type& timer_fd){
			if (!timer_fd) 
				return nullptr;
			
			_timer_queue.remove(timer_fd);
			operation_type* op = timer_fd->op;
			timer_fd->op = nullptr;
			if (op)
				op->set(std::make_error_code(std::errc::operation_canceled), 0);

			update_wakeup_after();
			return op;
		}

		void update_wakeup_after(void){
			if (!_timer_queue.empty()) {
				auto now = std::chrono::system_clock::now();
				if (_timer_queue.top()->expired_at <= now) {
					_wakeup_after = std::chrono::milliseconds(0);
				}
				else {
					_wakeup_after = std::chrono::duration_cast<std::chrono::milliseconds>(
						_timer_queue.top()->expired_at - now) + std::chrono::milliseconds(500);
				}
			}
			else {
				_wakeup_after = std::chrono::milliseconds(std::numeric_limits<int>::max());
			}
		}

	private:
		struct compare {
			bool operator()(descriptor_type& l, descriptor_type& r) {
				return l->expired_at > r->expired_at;
			}
		};
	private:
		engine_type& _engine;
		cx::stdext::removable_priority_queue<
			descriptor_type,
			std::vector<descriptor_type>,
			compare
		> _timer_queue;
		std::chrono::milliseconds _wakeup_after;
	};
}

#endif