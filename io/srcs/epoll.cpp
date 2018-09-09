/**
 * @brief
 *
 * @file epoll.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <cx/base/error.hpp>
#include <cx/io/mux/epoll.hpp>
#include <cx/io/basic_engine.hpp>

#if defined(CX_PLATFORM_LINUX)

namespace cx::io::mux {

	epoll::descriptor::descriptor(basic_engine<this_type>& e)
		: engine(e)
	{
		fd = invalid_socket;
	}

	epoll::epoll(basic_engine<this_type>& e)
		: _engine(e), _handle(-1), _eventfd(-1)
	{
		_handle = epoll_create(256);
		if (_handle == -1) {
			throw std::system_error(cx::system_error(), "epoll_create fails");
		}
		_eventfd = eventfd(0, 0);
		if (_eventfd == -1) {
			throw std::system_error(cx::system_error(), "eventfd fails");
		}
		epoll_event evt;
		evt.events = EPOLLIN;
		evt.data.ptr = nullptr;
		epoll_ctl(_handle, EPOLL_CTL_ADD, _eventfd, &evt);
	}

	epoll::~epoll(void) {
		close(_handle);
		close(_eventfd);
		_handle = -1;
		_eventfd = -1;
	}


	bool epoll::bind(const descriptor_type& descriptor) {
		std::error_code ec;
		return bind(descriptor, ec);
	}

	bool epoll::bind(const descriptor_type& descriptor, std::error_code& ec) {
		CX_UNUSED(descriptor);
		CX_UNUSED(ec);
		return true;
	}

	bool epoll::bind(const descriptor_type& descriptor, int ops) {
		std::error_code ec;
		return bind(descriptor, ops, ec);
	}

	bool epoll::bind(const descriptor_type& descriptor, int ops, std::error_code& ec) {
		if (_handle == -1) {
			ec = std::make_error_code(std::errc::bad_file_descriptor);
			return false;
		}
		if (descriptor.get() == nullptr || descriptor->fd == -1) {
			ec = std::make_error_code(std::errc::invalid_argument);
			return false;
		}

		epoll_event evt;
		evt.events = ops;
		evt.data.ptr = descriptor.get();
		if (epoll_ctl(_handle, EPOLL_CTL_MOD, descriptor->fd, &evt) == 0) {
			return true;
		}

		if ((errno == ENOENT) && ops) {
			if (epoll_ctl(_handle, EPOLL_CTL_ADD, descriptor->fd, &evt) == 0) {
				return true;
			}
		}
		ec = cx::system_error();
		return false;
	}

	void epoll::unbind(const descriptor_type& descriptor) {
		if (descriptor.get() == nullptr)
			return;
		if (descriptor->fd != -1) {
			epoll_event evt;
			evt.events = 0;
			evt.data.ptr = descriptor.get();
			epoll_ctl(_handle, EPOLL_CTL_DEL, descriptor->fd, &evt);
		}
	}

	void epoll::wakeup(void) {
		uint64_t v = 1;
		write(_eventfd, &v, sizeof(v));
	}

	int epoll::run(const std::chrono::milliseconds& wait_ms) {
		struct epoll_event events[256];
		int nbfd = epoll_wait(_handle
			, events
			, 256
			, wait_ms.count());
		if (nbfd <= 0) {
			return 0;
		}
		for (int i = 0; i < nbfd; ++i) {
			if (events[i].data.ptr) {
				bool changed = false;
				descriptor_type descriptor = static_cast<epoll::descriptor*>(events[i].data.ptr)->shared_from_this();
				int ops_filter[2] = { cx::io::pollin , cx::io::pollout };
				for (int j = 0; j < 2; ++j) {
					if (ops_filter[j] & events[i].events) {
						operation_type* op = descriptor->context[j].ops.head();
						if (op && op->complete(descriptor)) {
							descriptor->context[j].ops.remove_head();
							(*op)();
							if (descriptor->context[j].ops.empty())
								changed = true;
						}
					}
				}
				if (changed) {
					int ops = (descriptor->context[0].ops.empty() ? 0 : cx::io::pollin)
						| (descriptor->context[1].ops.empty() ? 0 : cx::io::pollout);
					std::error_code ec;
					if (bind(descriptor, ops, ec) == false) {
						if (ops != 0) {
							cx::slist<operation_type> postops(std::move(descriptor->context[0].ops));
							postops.add_tail(std::move(descriptor->context[1].ops));
						}
					}
				}
			}
			else {
				uint64_t v;
				read(_eventfd, &v, sizeof(v));
			}
		}
		return 0;
	}

	epoll::socket_type epoll::socket_handle(const epoll::descriptor_type& descriptor) {
		if (!descriptor)
			return invalid_socket;
		return descriptor->fd;
	}

	epoll::socket_type epoll::socket_handle(const epoll::descriptor_type& descriptor, 
		epoll::socket_type s)
	{
		if (!descriptor)
			return invalid_socket;
		std::swap(descriptor->fd, s);
		return s;
	}

	cx::slist<epoll::operation_type> epoll::drain_ops(
		const epoll::descriptor_type& descriptor,
		const std::error_code& ec)
	{
		cx::slist<epoll::operation_type> ops;
		if (!descriptor)
			return ops;
		ops.add_tail(std::move(descriptor->context[0].ops));
		ops.add_tail(std::move(descriptor->context[1].ops));
		auto op = ops.head();
		while (op) {
			op->set(ec, 0);
			op = op->next();
		}
		return ops;
	}
}

#endif