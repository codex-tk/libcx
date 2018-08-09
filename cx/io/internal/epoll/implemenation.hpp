/**
*/
#ifndef __cx_io_internal_epoll_h__
#define __cx_io_internal_epoll_h__
/*
*/
#include <cx/cxdefine.hpp>
#include <cx/slist.hpp>
#include <cx/container_of.hpp>
#include <cx/error.hpp>

#include <cx/io/internal/reactor/basic_reactor.hpp>
#include <cx/io/internal/basic_implementation.hpp>

#include <mutex>
#include <set>

#if defined(CX_PLATFORM_LINUX)

namespace cx::io::internal::epoll {

	class implementation : public cx::io::internal::basic_implementation {
	public:
		using reactor = cx::io::internal::reactor::basic_reactor<implementation>;
		using basic_handle = reactor::basic_handle;
		using handle_type = reactor::handle_type;
		using operation_type = reactor::operation_type;
		using native_handle_type = reactor::native_handle_type;

		implementation(void)
			: _handle(-1)
			, _eventfd(-1)
		{
			_handle = epoll_create(256);
			if (_handle == -1) {
				throw std::system_error(cx::system_error(), "epoll_create fails");
			}
			_eventfd = eventfd(0, 0);
			if (_eventfd == -1) {
				throw std::system_error(cx::system_error(), "eventfd fails");
			}
			_active_links.store(0);
			epoll_event evt;
			evt.events = EPOLLIN;
			evt.data.ptr = nullptr;
			epoll_ctl(_handle, EPOLL_CTL_ADD, _eventfd, &evt);
		}

		~implementation(void) {
			close(_handle);
			close(_eventfd);
			_handle = -1;
			_eventfd = -1;
		}

		bool bind(const handle_type& ptr, const int ops) {
			if (ptr.get() == nullptr || ptr->fd == -1) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}

			epoll_event evt;
			evt.events = ops;
			evt.data.ptr = ptr.get();
			if (epoll_ctl(_handle, EPOLL_CTL_MOD, ptr->fd, &evt) == 0) {
				return true;
			}

			if ((errno == ENOENT) && ops) {
				if (epoll_ctl(_handle, EPOLL_CTL_ADD, ptr->fd, &evt) == 0) {
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_active_handles.insert(ptr);
					return true;
				}
			}

			this->last_error(cx::system_error());
			return false;
		}

		void unbind(const handle_type& ptr) {
			if (ptr.get() == nullptr)
				return;
			if (ptr->fd != -1) {
				epoll_event evt;
				evt.events = 0;
				evt.data.ptr = ptr.get();
				epoll_ctl(_handle, EPOLL_CTL_DEL, ptr->fd, &evt);
			}			
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			_active_handles.erase(ptr);
		}

		void add_active_links(void) {
			_active_links += 1;
		}

		void release_active_links(void) {
			_active_links -= 1;
		}

		void run(void) {
			while (_active_links.load() != 0 || _active_handles.size() != 0) {
				run(std::chrono::milliseconds(0xffffffff));
			}
		}

		int run(const std::chrono::milliseconds& ms) {
			struct epoll_event events[256];
			int nbfd = epoll_wait(_handle
				, events
				, 256
				, ms.count());
			if (nbfd <= 0) { return 0; }
			cx::io::internal::basic_implementation::scoped_loop sl(*this);
			int proc = 0;
			for (int i = 0; i < nbfd; ++i) {
				implementation::basic_handle* pbasic_handle = static_cast<implementation::basic_handle*>(events[i].data.ptr);
				if (pbasic_handle) {
					proc += pbasic_handle->handle_events(*this, events[i].events);
				} else {
					uint64_t v;
					read(_eventfd, &v, sizeof(v));
					cx::slist< operation_type > ops;
					do {
						std::lock_guard<std::recursive_mutex> lock(_mutex);
						_ops.swap(ops);
						release_active_links();
					} while (0);
					while (operation_type* op = ops.head()) {
						ops.remove_head();
						proc += (*op)();
					}
				}
			}
			return proc;
		}

		void post(operation_type* op) {
			do {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				bool empty = _ops.empty();
				_ops.add_tail(op);
				if (empty) {
					add_active_links();
				}
			} while (0);
			uint64_t v = 1;
			write(_eventfd, &v, sizeof(v));
		}

		void post(cx::slist<operation_type>&& ops) {
			do {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				bool empty = _ops.empty();
				_ops.add_tail(std::forward<cx::slist<operation_type>>(ops));
				if (empty) {
					add_active_links();
				}
			} while (0);
			uint64_t v = 1;
			write(_eventfd, &v, sizeof(v));
		}

		template < typename HandlerT >
		void post_handler(HandlerT&& handler) {
			post(new handler_op<HandlerT>(std::forward<HandlerT>(handler)));
		}
	private:
		int _handle;
		int _eventfd;
		std::recursive_mutex _mutex;
		cx::slist<operation_type> _ops;
		std::set<handle_type> _active_handles;
		std::atomic<int> _active_links;
	private:
		template <typename HandlerT>
		class handler_op : public operation_type {
		public:
			handler_op(HandlerT&& handler)
				: _handler(std::forward<HandlerT>(handler))
			{}
			virtual ~handler_op(void) override {
			}
			virtual int operator()(void) override {
				_handler();
				delete this;
				return 1;
			}
		private:
			HandlerT _handler;
		};
	};

}

#endif

#endif

