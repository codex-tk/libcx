/**
 * @brief
 *
 * @file completion_port.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <cx/base/error.hpp>
#include <cx/io/io.hpp>
#include <cx/io/mux/completion_port.hpp>
#include <cx/io/basic_engine.hpp>
#include <cx/base/utils.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io::mux {

	completion_port::descriptor::descriptor(basic_engine<this_type>& e)
		: engine(e)
	{
		fd.s = invalid_socket;
	}

	completion_port::completion_port(basic_engine<this_type>& e)
		: _engine(e), _handle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1)) {
		if (_handle == INVALID_HANDLE_VALUE) {
			throw std::system_error(cx::system_error(), "CreateIoCompletionPort fails");
		}
	}

	completion_port::~completion_port(void) {
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}

	bool completion_port::bind(const descriptor_type& descriptor) {
		std::error_code ec;
		return bind(descriptor, ec);
	}

	bool completion_port::bind(const descriptor_type& descriptor, std::error_code& ec) {
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

	bool completion_port::bind(const descriptor_type& descriptor, int ops) {
		std::error_code ec;
		return bind(descriptor, ops, ec);
	}

	bool completion_port::bind(const descriptor_type&, int, std::error_code&) {
		return true;
	}

	void completion_port::unbind(const descriptor_type&) {
		
	}

	void completion_port::wakeup(void) {
		::PostQueuedCompletionStatus(_handle, 0, 0, nullptr);
	}

	int completion_port::run(const std::chrono::milliseconds& wait_ms) {
		LPOVERLAPPED ov = nullptr;
		DWORD bytes_transferred = 0;
		ULONG_PTR key = 0;
		BOOL ret = GetQueuedCompletionStatus(_handle, &bytes_transferred, &key,
			&ov, static_cast<DWORD>(wait_ms.count()));

		if (ov != nullptr && key != 0) {
			descriptor_type descriptor = reinterpret_cast<
				descriptor_type::element_type*>(key)->shared_from_this();
			std::error_code ec(ret == FALSE ? cx::system_error() : std::error_code());
			operation_type* op = cx::clazz<operation_type>::container_of(ov, &operation_type::ov);
			if (op) {
				op->set(ec, bytes_transferred);
				if (op->complete(descriptor)) {
					(*op)();
					return 1;
				}
			}
		}
		return 0;
	}

	completion_port::socket_type completion_port::socket_handle(
		const completion_port::descriptor_type& descriptor)
	{
		if (!descriptor)
			return invalid_socket;
		return descriptor->fd.s;
	}

	completion_port::socket_type completion_port::socket_handle(
		const completion_port::descriptor_type& descriptor, completion_port::socket_type s) {
		if (!descriptor)
			return invalid_socket;
		std::swap(descriptor->fd.s, s);
		return s;
	}

	cx::slist<completion_port::operation_type> completion_port::drain_ops(
		const completion_port::descriptor_type& ,
		const std::error_code& )
	{
		return cx::slist<completion_port::operation_type>{};
	}
}

#endif