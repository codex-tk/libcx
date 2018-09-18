/**
 * @brief 
 * 
 * @file win32_overlapped_stream_service..hpp
 * @author ghtak
 * @date 2018-09-01
 */

#ifndef __cx_io_ip_win32_overlapped_stream_service_h__
#define __cx_io_ip_win32_overlapped_stream_service_h__

#include <cx/base/defines.hpp>
#include <cx/base/error.hpp>
#include <cx/io/io.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/services/basic_service.hpp>
#include <cx/io/ops/basic_connect_operation.hpp>
#include <cx/io/ops/basic_read_operation.hpp>
#include <cx/io/ops/basic_write_operation.hpp>
#include <cx/io/ops/basic_accept_operation.hpp>
#include <cx/io/ops/handler_operation.hpp>

#include <cx/io/mux/completion_port.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io::ip {

	template < typename EngineType, int Type = SOCK_STREAM, int Proto = IPPROTO_TCP > 
	class win32_overlapped_stream_service {
	public:
		using this_type = win32_overlapped_stream_service;
		using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using operation_type = typename mux_type::operation_type;
		using descriptor_type = typename mux_type::descriptor_type;
		using basic_service_type = cx::io::ip::basic_service<engine_type>;
		using address_type = cx::io::ip::basic_address<Type, Proto>;
		using buffer_type = cx::io::buffer;

		using read_operation = cx::io::basic_read_operation<this_type, operation_type>;
		using readn_operation = cx::io::readn_operation<this_type, operation_type>;
		using write_operation = cx::io::basic_write_operation<this_type, operation_type>;

		template <typename HandlerType>
		using connect_operation = cx::io::ip::basic_connect_operation<HandlerType, this_type, operation_type>;

		class overlapped_operation : public operation_type {
		public:
			OVERLAPPED* overlapped() {
				_overlapped.operation = this;
				return _overlapped.ptr();
			}
		private:
			cx::io::mux::OVERLAPPEDEX_OP<descriptor_type, operation_type> _overlapped;
		};

		template <typename HandlerType>
		using accept_operation = cx::io::ip::basic_accept_operation<HandlerType, 
			this_type, overlapped_operation>;

		template <typename HandlerType>
		static void read(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			HandlerType&& handler)
		{
			read_operation* op = new handler_operation<read_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());

			const int type = 0;

			bool need_request = descriptor->ops[type].empty();
			descriptor->ops[type].add_tail(op);

			if (!mux_type::good(descriptor)) {
				descriptor->engine.post(mux_type::drain_ops(descriptor,
					std::make_error_code(std::errc::bad_file_descriptor)));
				return;
			}

			if (need_request)
				read_request(descriptor, op);
		}

		static void read_request(
			const descriptor_type& descriptor,
			read_operation* op)
		{
			decltype(descriptor->overlapped[0])& overlapped{ descriptor->overlapped[0] };
			overlapped.hold(descriptor);
			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSARecv(mux_type::socket_handle(descriptor)
				, op->buffer().raw_buffer()
				, 1
				, &bytes_transferred
				, &flag
				, overlapped.ptr()
				, nullptr) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING) {
					overlapped.unhold();
					descriptor->engine.post(mux_type::drain_ops(descriptor,
							cx::system_error()));
					return;
				}
			}
		}

		static bool read_complete(
			const descriptor_type& descriptor,
			read_operation* op)
		{
			CX_UNUSED(descriptor);
			CX_UNUSED(op);
			return true;
		}

		template <typename HandlerType>
		static void readn(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			HandlerType&& handler)
		{
			readn_operation* op = new handler_operation<readn_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());

			const int type = 0;

			bool need_request = descriptor->ops[type].empty();
			descriptor->ops[type].add_tail(op);

			if (!mux_type::good(descriptor)) {
				descriptor->engine.post(mux_type::drain_ops(descriptor,
					std::make_error_code(std::errc::bad_file_descriptor)));
				return;
			}

			if (need_request)
				read_request(descriptor, op);
		}

		static void read_request(
			const descriptor_type& descriptor,
			readn_operation* op)
		{
			decltype(descriptor->overlapped[0])& overlapped{ descriptor->overlapped[0] };
			overlapped.hold(descriptor);
			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSARecv(mux_type::socket_handle(descriptor)
				, op->buffer().raw_buffer()
				, 1
				, &bytes_transferred
				, &flag
				, overlapped.ptr()
				, nullptr) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING) {
					overlapped.unhold();
					descriptor->engine.post(mux_type::drain_ops(descriptor,
						cx::system_error()));
					return;
				}
			}
		}

		static bool read_complete(
			const descriptor_type& descriptor,
			readn_operation* op)
		{
			if (op->error() || op->size() <= 0) {
				op->set(op->error(), op->total_read_size());
				return true;
			}

			op->consume(op->size());

			if (op->buffer().length() == 0) {
				op->set(op->error(), op->total_read_size());
				return true;
			}
			read_request(descriptor, op);
			return false;
		}

		template <typename HandlerType>
		static void write(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			HandlerType&& handler)
		{
			write_operation* op = new handler_operation<write_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());

			const int type = 1;

			bool need_request = descriptor->ops[type].empty();
			descriptor->ops[type].add_tail(op);

			if (!mux_type::good(descriptor)) {
				descriptor->engine.post(mux_type::drain_ops(descriptor,
					std::make_error_code(std::errc::bad_file_descriptor)));
				return;
			}

			if (need_request)
				write_request(descriptor, op);
		}

		static void write_request(
			const descriptor_type& descriptor,
			write_operation* op)
		{
			decltype(descriptor->overlapped[1])& overlapped{ descriptor->overlapped[1] };
			overlapped.hold(descriptor);

			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSASend(mux_type::socket_handle(descriptor)
				, op->buffer().raw_buffer()
				, 1
				, &bytes_transferred
				, flag
				, overlapped.ptr()
				, nullptr) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING) {
					overlapped.unhold();
					descriptor->engine.post(mux_type::drain_ops(descriptor,
						cx::system_error()));
					return;
				}
			}
		}

		static bool write_complete(
			const descriptor_type& descriptor,
			write_operation* op)
		{
			CX_UNUSED(descriptor);
			CX_UNUSED(op);
			return true;
		}

		template <typename HandlerType>
		static void connect(
			const descriptor_type& descriptor,
			const address_type& addr,
			HandlerType&& handler)
		{
			connect_operation<HandlerType>* op = new connect_operation<HandlerType>(addr,
				std::forward<HandlerType>(handler));

			const int type = 1;

			bool need_request = descriptor->ops[type].empty();
			descriptor->ops[type].add_tail(op);

			if (!mux_type::good(descriptor)) {
				descriptor->engine.post(mux_type::drain_ops(descriptor,
					std::make_error_code(std::errc::bad_file_descriptor)));
				return;
			}

			if (need_request)
				connect_request(descriptor, op);
		}

		template <typename HandlerType>
		static void connect_request(
			const descriptor_type& descriptor,
			connect_operation<HandlerType>* op)
		{
			decltype(descriptor->overlapped[1])& overlapped{ descriptor->overlapped[1] };
			overlapped.hold(descriptor);

			LPFN_CONNECTEX _connect_ex = nullptr;
			DWORD bytes_returned = 0;
			GUID guid = WSAID_CONNECTEX;
			if (WSAIoctl(mux_type::socket_handle(descriptor)
				, SIO_GET_EXTENSION_FUNCTION_POINTER
				, &guid, sizeof(guid)
				, &_connect_ex, sizeof(_connect_ex)
				, &bytes_returned, nullptr, nullptr) != SOCKET_ERROR)
			{
				std::error_code ec;
				address_type bindaddr = address_type::any(0, op->address().family());
				if (basic_service_type::bind(descriptor, bindaddr, ec)) {
					bytes_returned = 0;
					if (_connect_ex(mux_type::socket_handle(descriptor)
						, op->address().sockaddr()
						, op->address().length()
						, nullptr, 0
						, &bytes_returned
						, overlapped.ptr()) == TRUE)
						return;
					if (WSAGetLastError() == WSA_IO_PENDING) {
						return;
					}
				}
			}
			overlapped.unhold();
			descriptor->engine.post(mux_type::drain_ops(descriptor, cx::system_error()));
		}

		template <typename HandlerType>
		static bool connect_complete(
			const descriptor_type& descriptor,
			connect_operation<HandlerType>* op)
		{
			CX_UNUSED(descriptor);
			CX_UNUSED(op);
			return true;
		}

		template <typename HandlerType>
		static void accept(
			const descriptor_type& descriptor,
			const descriptor_type& accepted,
			HandlerType&& handler)
		{
			accept_operation<HandlerType>* op = new accept_operation<HandlerType>(accepted,
				std::forward<HandlerType>(handler));
			if (!mux_type::good(descriptor)) {
				op->set(std::make_error_code(std::errc::bad_file_descriptor), 0);
				descriptor->engine.post(op);
				return;
			}

			address_type addr;
			std::error_code ec;
			basic_service_type::local_address(descriptor, addr.sockaddr(), addr.length_ptr(), ec);

			if (basic_service_type::open(accepted, addr, ec)) {
				if (accepted->engine.multiplexer().bind(accepted, ec)){
					DWORD bytes_returned = 0;
					if (AcceptEx(mux_type::socket_handle(descriptor)
						, mux_type::socket_handle(accepted)
						, op->address().sockaddr()
						, 0
						, 0
						, op->address().length()
						, &bytes_returned
						, op->overlapped()) == TRUE) {
						return;
					}
					if (WSAGetLastError() == WSA_IO_PENDING)
						return;
				}
			}
			op->set(cx::system_error(), 0);
			descriptor->engine.post(op);
			return;
		}

		template <typename HandlerType>
		static void accept_request(
			const descriptor_type& descriptor,
			accept_operation<HandlerType>* op)
		{
			CX_UNUSED(descriptor);
			CX_UNUSED(op);
		}

		template <typename HandlerType>
		static bool accept_complete(
			const descriptor_type& descriptor,
			accept_operation<HandlerType>* op)
		{
			CX_UNUSED(descriptor);
			CX_UNUSED(op);
			return true;
		}
	};
}

#endif

#endif