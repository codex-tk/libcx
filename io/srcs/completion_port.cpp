/**
 * @brief
 *
 * @file completion_port.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <cx/base/error.hpp>
#include <cx/io/mux/completion_port.hpp>
#include <cx/io/basic_engine.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io::mux {

	completion_port::completion_port(basic_engine<completion_port>* e)
		: _engine(e), _handle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1)) {
		if (_handle == INVALID_HANDLE_VALUE) {
			throw std::system_error(cx::system_error(), "CreateIoCompletionPort fails");
		}
	}

	completion_port::~completion_port(void) {
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}

	bool completion_port::bind(const descriptor_ptr& descriptor) {
		std::error_code ec;
		return bind(descriptor, ec);
	}

	bool completion_port::bind(const descriptor_ptr& descriptor, std::error_code& ec) {
		if (_handle == INVALID_HANDLE_VALUE) {
			ec = std::make_error_code(std::errc::bad_file_descriptor);
			return false;
		}
		if (descriptor->fd.s == INVALID_SOCKET) {
			ec = std::make_error_code(std::errc::invalid_argument);
			return false;
		}
		if (CreateIoCompletionPort(
			descriptor->fd.h
			, _handle
			, reinterpret_cast<ULONG_PTR>(descriptor.get())
			, 0) != _handle)
		{
			ec = cx::system_error();
			return false;
		}
		return true;
	}

	bool completion_port::bind(const descriptor_ptr& descriptor, int ops) {
		std::error_code ec;
		return bind(descriptor, ops, ec);
	}

	bool completion_port::bind(const descriptor_ptr& descriptor, int ops, std::error_code& ec) {
		CX_UNUSED(ops);
		CX_UNUSED(descriptor);
		CX_UNUSED(ec);
		return true;
	}

	void completion_port::unbind(const descriptor_ptr& descriptor) {
		CX_UNUSED(descriptor);
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
			descriptor::OVERLAPPEDEX* povex = static_cast<descriptor::OVERLAPPEDEX*>(ov);
			int ctx_idx = povex->type >> 2;
			if (ctx_idx >= 0 && ctx_idx <= 1) {
				descriptor_ptr descriptor = reinterpret_cast<
					descriptor_ptr::element_type*>(key)->shared_from_this();
				operation* op = descriptor->context[ctx_idx].ops.head();
				if (op) {
					op->set(ret == FALSE ? cx::system_error() : std::error_code(), bytes_transferred);
					if (op->complete(descriptor)) {
						descriptor->context[ctx_idx].ops.remove_head();
						(*op)();
						if (descriptor->context[ctx_idx].ops.head()) {
							descriptor->context[ctx_idx].ops.head()->request(descriptor);
						}
					}
				}
			}
		}
		return 0;
	}
	
}

#endif