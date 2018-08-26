/**
 * @brief
 *
 * @file epoll.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <cx/base/error.hpp>

#include <cx/io/internal/epoll.hpp>
#include <cx/io/operation.hpp>

#if defined(CX_PLATFORM_LINUX)

namespace cx::io::internal {

	epoll::epoll(cx::io::engine& e)
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

	bool epoll::bind(const cx::io::descriptor_t& fd, int ops) {
		if (fd.get() == nullptr || fd->fd<int>() == -1) {
			return false;
		}

		epoll_event evt;
		evt.events = ops;
		evt.data.ptr = fd.get();
		if (epoll_ctl(_handle, EPOLL_CTL_MOD, fd->fd<int>(), &evt) == 0) {
			return true;
		}

		if ((errno == ENOENT) && ops) {
			if (epoll_ctl(_handle, EPOLL_CTL_ADD, fd->fd<int>(), &evt) == 0) {
				return true;
			}
		}
		return false;
	}

	void epoll::unbind(const cx::io::descriptor_t& fd) {
		if (fd.get() == nullptr)
			return;
		if (fd->fd<int>() != -1) {
			epoll_event evt;
			evt.events = 0;
			evt.data.ptr = fd.get();
			epoll_ctl(_handle, EPOLL_CTL_DEL, fd->fd<int>(), &evt);
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
		if (nbfd <= 0)
			return 0;
		for (int i = 0; i < nbfd; ++i) {
			cx::io::descriptor* pfd = static_cast<cx::io::descriptor*>(events[i].data.ptr);
			if (pfd) {
				pfd->handle_event(_engine, events[i].events);
			}
			else {
				uint64_t v;
				read(_eventfd, &v, sizeof(v));
			}
		}
		return 0;
	}
}

#endif