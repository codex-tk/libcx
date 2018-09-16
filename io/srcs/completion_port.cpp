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
		overlapped[0].type = cx::io::pollin;
		overlapped[1].type = cx::io::pollout;
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

			OVERLAPPEDEX<descriptor_type>* ovex = static_cast<OVERLAPPEDEX<descriptor_type>*>(ov);

			switch (ovex->type)
			{
			case cx::io::pollin: 
				return handle_event(descriptor, 0, ec, bytes_transferred);
			case cx::io::pollout: 
				return handle_event(descriptor, 1, ec, bytes_transferred);
			case cx::io::pollop:
				return handle_event(descriptor,
					static_cast<OVERLAPPEDEX_OP<descriptor_type, operation_type>*>(ov)->operation,
					ec,
					bytes_transferred);
				break;
			default:
				assert(0);
				break;
			}
		}
		return 0;
	}

	int completion_port::handle_event(const descriptor_type& descriptor, int type,
		const std::error_code& ec, DWORD byte_transferred)
	{
		descriptor->overlapped[type].descriptor = nullptr;
		operation_type* op = descriptor->ops[type].head();
		if (op) {
			op->set(ec, byte_transferred);
			if (op->complete(descriptor)) {
				descriptor->ops[type].remove_head();
				bool need_request = descriptor->ops[type].empty() == false;
				(*op)();
				if (need_request) {
					descriptor->ops[type].head()->request(descriptor);
				}
				return 1;
			}
		}
		return 0;
	}

	int completion_port::handle_event(const descriptor_type& descriptor, operation_type* op,
		const std::error_code& ec, DWORD byte_transferred)
	{
		if (op) {
			op->set(ec, byte_transferred);
			if (op->complete(descriptor)) {
				(*op)();
				return 1;
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
		const completion_port::descriptor_type& descriptor,
		const std::error_code& ec)
	{
		cx::slist<completion_port::operation_type> ops;
		if (!descriptor)
			return ops;
		ops.add_tail(std::move(descriptor->ops[0]));
		ops.add_tail(std::move(descriptor->ops[1]));
		auto op = ops.head();
		while (op) {
			op->set(ec, 0);
			op = op->next();
		}
		return ops;
	}
}

#endif