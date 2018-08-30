/**
 * @brief
 *
 * @file completion_port.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <cx/base/error.hpp>

#include <cx/io/mux/completion_port.hpp>
#include <cx/io/operation.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io::mux {

	completion_port::completion_port(cx::io::engine& e)
		: _engine(e),
		_handle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1)) {
		if (_handle == INVALID_HANDLE_VALUE) {
			throw std::system_error(cx::system_error(), "CreateIoCompletionPort fails");
		}
	}

	completion_port::~completion_port(void) {
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}

	bool completion_port::bind(const cx::io::descriptor_t& fd) {
		std::error_code ec;
		return bind(fd, ec);
	}

	bool completion_port::bind(const cx::io::descriptor_t& fd, std::error_code& ec) {
		if (_handle == INVALID_HANDLE_VALUE) {
			ec = std::make_error_code(std::errc::bad_file_descriptor);
			return false;
		}
		if (fd->native_handle<SOCKET>() == INVALID_SOCKET) {
			ec = std::make_error_code(std::errc::invalid_argument);
			return false;
		}
		if (CreateIoCompletionPort(
			fd->native_handle<HANDLE>()
			, _handle
			, reinterpret_cast<ULONG_PTR>(fd.get())
			, 0) != _handle)
		{
			ec = cx::system_error();
			return false;
		}
		return true;
	}

	bool completion_port::bind(const cx::io::descriptor_t& fd, int ops) {
		std::error_code ec;
		return bind(fd, ops, ec);
	}

	bool completion_port::bind(const cx::io::descriptor_t& fd, int ops, std::error_code& ec) {
		CX_UNUSED(ops);
		CX_UNUSED(fd);
		CX_UNUSED(ec);
		return true;
	}

	void completion_port::unbind(const cx::io::descriptor_t& fd) {
		CX_UNUSED(fd);
	}

	void completion_port::wakeup(void) {
		::PostQueuedCompletionStatus(_handle, 0, 0, nullptr);
	}

	int completion_port::run(const std::chrono::milliseconds& wait_ms) {
		LPOVERLAPPED ov = nullptr;
		DWORD bytes_transferred = 0;
		ULONG_PTR key;
		BOOL ret = GetQueuedCompletionStatus(_handle, &bytes_transferred, &key,
			&ov, static_cast<DWORD>(wait_ms.count()));
		if (ov != nullptr && key != 0) {
			cx::io::operation* op = cx::io::operation::container_of(ov);
			std::error_code ec = ret == FALSE ? cx::system_error() : std::error_code();
			op->set(ec, bytes_transferred);
			reinterpret_cast<cx::io::descriptor*>(key)->handle_event(_engine, op->type());
		}
		return 0;
	}
}

#endif