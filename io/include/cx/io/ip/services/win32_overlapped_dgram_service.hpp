/**
 * @brief
 *
 * @file win32_overlapped_dgram_service..hpp
 * @author ghtak
 * @date 2018-09-01
 */

#ifndef __cx_io_ip_win32_overlapped_dgram_service_h__
#define __cx_io_ip_win32_overlapped_dgram_service_h__

#include <cx/base/defines.hpp>
#include <cx/base/error.hpp>
#include <cx/io/io.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/services/basic_service.hpp>
#include <cx/io/ops/basic_dgram_operation.hpp>
#include <cx/io/ops/basic_read_operation.hpp>
#include <cx/io/ops/basic_write_operation.hpp>
#include <cx/io/ops/handler_operation.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io::ip {

	template < typename EngineType, int Type = SOCK_DGRAM, int Proto = IPPROTO_UDP > 
	class win32_overlapped_dgram_service {
	public:
		using this_type = win32_overlapped_dgram_service;
		using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using operation_type = typename mux_type::operation_type;
		using descriptor_type = typename mux_type::descriptor_type;
		using basic_service_type = cx::io::ip::basic_service<engine_type>;
		using address_type = cx::io::ip::basic_address<Type, Proto>;
		using buffer_type = cx::io::buffer;

		using dgram_operation = cx::io::ip::basic_dgram_operation<this_type, operation_type>;

		using read_operation = cx::io::basic_read_operation<this_type, dgram_operation>;
		using write_operation = cx::io::basic_write_operation<this_type, dgram_operation>;

		template <typename HandlerType>
		static void read(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			const address_type& addr,
			HandlerType&& handler)
		{
			read_operation* op = new dgram_handler_operation<read_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());
			op->address() = addr;

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

		template <typename HandlerType>
		static void write(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			const address_type& addr,
			HandlerType&& handler)
		{
			write_operation* op = new dgram_handler_operation<write_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());
			op->address() = addr;

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

		static void read_request(
			const descriptor_type& descriptor,
			read_operation* op)
		{	
			decltype(descriptor->overlapped[0])& overlapped{ descriptor->overlapped[0] };
			overlapped.hold(descriptor);
			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSARecvFrom(mux_type::socket_handle(descriptor)
				, op->buffer().raw_buffer()
				, 1
				, &bytes_transferred
				, &flag
				, op->address().sockaddr()
				, op->address().length_ptr()
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

		static void write_request(
			const descriptor_type& descriptor,
			write_operation* op)
		{
			decltype(descriptor->overlapped[1])& overlapped{ descriptor->overlapped[1] };
			overlapped.hold(descriptor);
			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSASendTo(mux_type::socket_handle(descriptor)
				, op->buffer().raw_buffer()
				, 1
				, &bytes_transferred
				, flag
				, op->address().sockaddr()
				, op->address().length()
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
	};
}

#endif

#endif