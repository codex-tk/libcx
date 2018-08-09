/**
 */

#ifndef _cx_time_detail_win32_timer_queue_service_h__
#define _cx_time_detail_win32_timer_queue_service_h__

#include <cx/cxdefine.hpp>
#include <cx/stdext/removable_priority_queue.hpp>

namespace cx::time {

#if defined(CX_PLATFORM_WIN32)

	template < typename ImplementationType, std::size_t Resolution  >
	class win32_timer_queue_service {
	public:
		using implementation_type = ImplementationType;

		struct handle {
			std::chrono::system_clock::time_point expired_at;
#if 0
			std::chrono::system_clock::time_point order;
#endif
			std::function< void(const std::error_code&) > handler;
		};
		using handle_type = std::shared_ptr<handle>;

		struct handle_type_cmp {
			bool operator()(handle_type t, handle_type u) {
#if 0
				if (t->expired_at == u->expired_at) {
					return t->order > u->order;
				}
#endif
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
				: _service(service) {}

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
			if (_wakeup_timer == INVALID_HANDLE_VALUE) {
				throw std::system_error(cx::system_error(), "CreateTimerQueueTimer fails");
			}
		}

		~win32_timer_queue_service(void) {
			if (_wakeup_timer != INVALID_HANDLE_VALUE )
				::DeleteTimerQueueTimer(nullptr, _wakeup_timer, nullptr);
			_wakeup_timer = INVALID_HANDLE_VALUE;
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
#if 0
			handle->order = std::chrono::system_clock::now();
#endif
			if (_queue.empty())
				_implementation.add_active_links();
			_queue.push(handle);

		}

		int handle_timers(void) {
			std::vector< handle_type > handles;
			do {
				std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				while (!_queue.empty()) {
					if (_queue.top()->expired_at < now ) {
						handles.push_back(_queue.top());
						_queue.pop();
					} else {
						break;
					}
				}
				if (_queue.empty())
					_implementation.release_active_links();
			} while (0);
			for (auto handle : handles) {
				handle->handler(std::make_error_code(std::errc::timed_out));
			}
			return handles.size();
		}

		bool cancel(handle_type handle) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (_queue.remove(handle)) {
				if (_queue.empty())
					_implementation.release_active_links();
				_implementation.post_handler([handle] {
					handle->handler(std::make_error_code(std::errc::operation_canceled));
				});
				return true;
			}
			return false;
		}

	private:
		implementation_type& _implementation;
		timer_op _timer_op;
		HANDLE _wakeup_timer;
		cx::stdext::removable_priority_queue< handle_type
			, std::vector<handle_type>
			, handle_type_cmp > _queue;
		std::recursive_mutex _mutex;
	};
#endif

}

#endif