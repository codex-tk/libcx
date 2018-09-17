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
			if (std::chrono::system_clock::now() > _timer_queue.top()->expired_at)
				_wakeup_after = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now() - _timer_queue.top()->expired_at);
			else
				_wakeup_after = std::chrono::milliseconds(0);
		}

		std::chrono::milliseconds wakeup_after(void) {
			return _wakeup_after;
		}

		cx::slist<operation_type> drain_expired_timers(void) {
			cx::slist<operation_type> ops;
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			while (!_timer_queue.empty()) {
				if (std::chrono::system_clock::to_time_t(_timer_queue.top()->expired_at) <= now) {
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
			if (!_timer_queue.empty()) {
				if (std::chrono::system_clock::now() > _timer_queue.top()->expired_at)
					_wakeup_after = std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now() - _timer_queue.top()->expired_at);
				else
					_wakeup_after = std::chrono::milliseconds(0);
			}
			else {
				_wakeup_after = std::chrono::milliseconds(std::numeric_limits<int>::max());
			}
			return ops;
		}

		operation_type* cancel(const descriptor_type& timer_fd){
			operation_type* op = timer_fd->op;
			_timer_queue.remove(timer_fd);
			timer_fd->op = nullptr;
			return op;
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