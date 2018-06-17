/**
 */
#ifndef _cx_time_detail_reactor_timer_fd_service_h__
#define _cx_time_detail_reactor_timer_fd_service_h__

#include <cx/cxdefine.hpp>
#include <cx/stdext/removable_priority_queue.hpp>

#if CX_PLATFORM == CX_P_LINUX

#include <sys/timerfd.h>

namespace cx::time {

	template < typename ImplementationType >
	class reactor_timer_fd_service {
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
			timer_op(reactor_timer_fd_service& svc) 
				: _service(svc) {}

			virtual ~timer_op(void) {}

			virtual bool complete(const typename implementation_type::handle_type& handle) {
				uint64_t num_expirations = 0;
				read(handle->fd, &num_expirations, sizeof(num_expirations));
				return true;
			};

			virtual int operator()(void) override {
				return _service.handle_timers();
			}
		private:
			reactor_timer_fd_service& _service;
		};

		explicit reactor_timer_fd_service(implementation_type& impl)
			: _implementation(impl)
			, _timer_op(*this)
			, _timer_fd(std::make_shared< typename implementation_type::basic_handle >())
		{
			_timer_fd->fd = timerfd_create(CLOCK_REALTIME, 0);
			if (_timer_fd->fd == -1) {
				throw std::system_error(cx::system_error(), "timerfd_create fails");
			}
		}

		~reactor_timer_fd_service(void) {
			::close(_timer_fd->fd);
		}

		void set_wakeup_time(void) {
			struct itimerspec its;
			memset(&its, 0x00, sizeof(its));
			auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch());
			auto wakeup = std::chrono::duration_cast<std::chrono::milliseconds>(
				_queue.top()->expired_at.time_since_epoch());
			auto diff = wakeup - now;
			if (diff.count() > 0) {
				its.it_value.tv_sec = (diff.count() / 1000);
				its.it_value.tv_nsec = (diff.count() % 1000) * 1000;
			} else {
				its.it_value.tv_nsec = 1;
			}
			timerfd_settime(_timer_fd->fd, 0, &its, NULL);
		}

		void fire(handle_type handle) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
#if 0
			handle->order = std::chrono::system_clock::now();
#endif
			if (_queue.empty()) {
				_implementation.add_active_links();
				_timer_fd->ops[0].add_tail(&_timer_op);
				_implementation.bind(_timer_fd, cx::io::pollin);
			}
			_queue.push(handle);
			set_wakeup_time();
		}

		int handle_timers(void) {
			std::vector< handle_type > handles;
			do {
				std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				while (!_queue.empty()) {
					if (std::chrono::system_clock::to_time_t(_queue.top()->expired_at) <= now) {
						handles.push_back(_queue.top());
						_queue.pop();
					} else {
						break;
					}
				}
				if (_queue.empty()) {
					_implementation.release_active_links();
					_implementation.unbind(_timer_fd);
				} else {
					set_wakeup_time();
					_timer_fd->ops[0].add_tail(&_timer_op);
				}
			} while (0);
			for (auto handle : handles) {
				handle->handler(std::make_error_code(std::errc::timed_out));
			}
			return handles.size();
		}

		bool cancel(handle_type handle) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (_queue.remove(handle)) {
				_implementation.post_handler([handle] {
					handle->handler(std::make_error_code(std::errc::operation_canceled));
				});
				return true;
			}
			return false;
		}
	private:
		std::recursive_mutex _mutex;
		implementation_type& _implementation;
		timer_op _timer_op;
		typename implementation_type::handle_type _timer_fd;
		cx::stdext::removable_priority_queue< handle_type
			, std::vector<handle_type>
			, handle_type_cmp > _queue;
	};
}

#endif

#endif