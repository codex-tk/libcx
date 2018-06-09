/**
 */

#ifndef _cx_time_detail_win32_timer_queue_service_h__
#define _cx_time_detail_win32_timer_queue_service_h__

#include <cx/cxdefine.hpp>


namespace cx::time::detail {

#if CX_PLATFORM == CX_P_WINDOWS

	template < typename ImplementationType, std::size_t Resolution  >
	class win32_timer_queue_service {
	public:
		using implementation_type = ImplementationType;

		struct handle {
			std::chrono::system_clock::time_point expired_at;
			//std::chrono::system_clock::time_point order;
			std::function< void(const std::error_code&) > handler;
		};
		using handle_type = std::shared_ptr<handle>;

		struct handle_type_cmp {
			bool operator()(handle_type t, handle_type u) {
				/*
				if (t->expired_at == u->expired_at) {
					return t->order > u->order;
				}*/
				return t->expired_at > u->expired_at;
			}
		};

		handle_type make_shared_handle(void) {
			return std::make_shared< handle>();
		}


		class timer_op
			: public implementation_type::operation_type {
		public:
			timer_op(win32_timer_queue_service& service)
				: _service(service)
			{
			}

			virtual ~timer_op(void) {}
			virtual int operator()(void) override {
				return _service.handle_timers();
			}
		private:
			win32_timer_queue_service& _service;
		};

		static VOID CALLBACK timer_callback(_In_ PVOID lpParameter, _In_ BOOLEAN /*TimerOrWaitFired*/) {
			win32_timer_queue_service* service = static_cast<win32_timer_queue_service*>(lpParameter);
			service->handle_timer_callback();
		}

		explicit win32_timer_queue_service(implementation_type& impl)
			: _implementation(impl)
			, _timer_op(*this)
			, _wakeup_timer(INVALID_HANDLE_VALUE)
		{
			::CreateTimerQueueTimer(&_wakeup_timer, nullptr
				, &win32_timer_queue_service::timer_callback
				, this
				, Resolution
				, Resolution
				, WT_EXECUTEDEFAULT);

		}

		~win32_timer_queue_service(void) {
			DeleteTimerQueueTimer(nullptr, _wakeup_timer, nullptr);
		}

		void handle_timer_callback(void) {
			do {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				if (_queue.empty())
					return;

				if (_queue.top()->expired_at > std::chrono::system_clock::now()) {
					return;
				}
			} while (0);
			_implementation.post(&_timer_op);
		}

		void fire(handle_type handle) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			//handle->order = std::chrono::system_clock::now();
			_queue.push(handle);
		}

		int handle_timers(void) {
			int index = 0;
			constexpr int max_drain = 256;
			std::array< handle_type, max_drain > timers;
			do {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				for (index = 0; index < max_drain; ++index) {
					if (_queue.empty())
						break;
					if (_queue.top()->expired_at < std::chrono::system_clock::now()) {
						timers[index] = _queue.top();
						_queue.pop();
					}
					else {
						break;
					}
				}
			} while (0);
			for (int i = 0; i < index; ++i) {
				timers[i]->handler(std::make_error_code(std::errc::timed_out));
			}
			return index;
		}

		void cancel(handle_type handle) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (_queue.remove(handle)) {
				_implementation.post_handler([handle] {
					handle->handler(std::make_error_code(std::errc::operation_canceled));
				});
			}
		}

		template<typename T, typename Contaner, typename Cmp >
		class removable_priority_queue
			: public std::priority_queue<T, Contaner, Cmp > {
		public:
			bool remove(const T& value) {
				auto it = std::find(this->c.begin(), this->c.end(), value);
				if (it != this->c.end()) {
					this->c.erase(it);
					std::make_heap(this->c.begin(), this->c.end(), this->comp);
					return true;
				}
				else {
					return false;
				}
			}
		};
	private:
		implementation_type& _implementation;
		timer_op _timer_op;
		HANDLE _wakeup_timer;
		removable_priority_queue< handle_type, std::vector<handle_type>, handle_type_cmp > _queue;
		std::recursive_mutex _mutex;
	};
#endif

}

#endif