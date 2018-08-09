/**
 */
#ifndef __cx_io_ip_iocp_socket_service_h__
#define __cx_io_ip_iocp_socket_service_h__

#if defined(CX_PLATFORM_WIN32)

#include <cx/io/internal/iocp/basic_socket_service.hpp>

#include <cx/io/basic_buffer.hpp>
#include <cx/io/internal/iocp/ops/read_op.hpp>
#include <cx/io/internal/iocp/ops/write_op.hpp>
#include <cx/io/internal/iocp/implementation.hpp>

#include <cx/io/ip/hdr.hpp>
#include <cx/io/ip/basic_address.hpp>

namespace cx::io::internal::iocp::ip {
	
	template < int Type, int Proto >
	class socket_service : public basic_socket_service <socket_service<Type,Proto>>{
	public:
		using this_type = socket_service;
		using base_type = basic_socket_service<this_type>;

		using implementation_type = typename base_type::implementation_type;
		using address_type = typename base_type::address_type;
		using handle_type = typename base_type::handle_type;
		using native_handle_type = typename base_type::native_handle_type;

		struct _buffer {
			_buffer(void* ptr, std::size_t len)
				: buffer(ptr, len){}
			_buffer(void)
				: buffer(nullptr, 0){}
			address_type address;
			cx::io::buffer buffer;
		};
		
		using buffer_type = _buffer;
		using basic_socket_service < this_type >::connect;

		template < typename HandlerType > using read_op 
			= cx::io::internal::iocp::ip::read_op< this_type , HandlerType >;
		template < typename HandlerType > using write_op
			= cx::io::internal::iocp::ip::write_op< this_type , HandlerType >;

		handle_type make_shared_handle(void) {
			return std::make_shared<_handle>(*this);
		}

		handle_type make_shared_handle(native_handle_type handle) {
			auto h = make_shared_handle();
			h->fd.s = handle;
			implementation().bind(h, 0);
			return h;
		}

		socket_service(implementation_type& impl)
			: basic_socket_service(impl) {}

		int write(const handle_type& handle, const buffer_type& buf) {
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				last_error(std::make_error_code(std::errc::invalid_argument));
				return -1;
			}
			int ret = sendto(handle->fd.s
				, static_cast<const char*>(buf.buffer.base())
				, buf.buffer.length()
				, 0
				, buf.address.sockaddr()
				, buf.address.length());

			if (ret == SOCKET_ERROR) {
				last_error(cx::system_error());
				return -1;
			}
			return ret;
		}

		int read(const handle_type& handle, buffer_type& buf) {
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				last_error(std::make_error_code(std::errc::invalid_argument));
				return -1;
			}
			int ret = recvfrom(handle->fd.s
				, static_cast<char*>(buf.buffer.base())
				, buf.buffer.length()
				, 0
				, buf.address.sockaddr()
				, buf.address.length_ptr());

			if (ret == SOCKET_ERROR) {
				last_error(cx::system_error());
				return -1;
			}
			return ret;
		}

		template < typename HandlerType >
		void async_write(const handle_type& handle, const buffer_type& buf, HandlerType&& handler) {
			write_op<HandlerType>* op =
				new write_op<HandlerType>(buf, std::forward<HandlerType>(handler));

			async_write(handle, op);
		}

		void async_write(const handle_type& handle, basic_write_op<this_type>* op) {
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				op->error(std::make_error_code(std::errc::invalid_argument));
				implementation().post(op);
				return;
			}

			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSASendTo(handle->fd.s
				, op->buffer().buffer.raw_buffer()
				, 1
				, &bytes_transferred
				, flag
				, op->buffer().address.sockaddr()
				, op->buffer().address.length()
				, op->overlapped()
				, nullptr) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING) {
					op->error(cx::system_error());					
					implementation().post(op);
					return;
				}
			}
			implementation().add_active_links();
		}

		template < typename HandlerType >
		void async_read(const handle_type& handle, const buffer_type& buf, HandlerType&& handler) {
			read_op<HandlerType>* op =
				new read_op<HandlerType>(buf, std::forward<HandlerType>(handler));

			async_read(handle, op);
		}

		void async_read(const handle_type& handle, basic_read_op<this_type>* op) {
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				op->error(std::make_error_code(std::errc::invalid_argument));
				implementation().post(op);
				return;
			}


			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSARecvFrom(handle->fd.s
				, op->buffer().buffer.raw_buffer()
				, 1
				, &bytes_transferred
				, &flag
				, op->buffer().address.sockaddr()
				, op->buffer().address.length_ptr()
				, op->overlapped()
				, nullptr) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSA_IO_PENDING) {
					op->error(cx::system_error());
					implementation().post(op);
					return;
				}
			}
			implementation().add_active_links();
		}

		using read_op_type = std::shared_ptr<basic_read_op<this_type>>;
		using write_op_type = std::shared_ptr<basic_write_op<this_type>>;

		template < typename HandlerType >
		read_op_type make_read_op(HandlerType&& handler) {
			return std::make_shared<cx::io::reusable_handler_op< read_op_type::element_type, HandlerType>>(
				std::forward<HandlerType>(handler)
				);
		}

		template < typename HandlerType >
		write_op_type make_write_op(HandlerType&& handler) {
			return std::make_shared<cx::io::reusable_handler_op< write_op_type::element_type, HandlerType>>(
				std::forward<HandlerType>(handler)
				);
		}	

		void async_read(const handle_type& handle, read_op_type op) {
			async_read(handle, op.get());
		}

		void async_write(const handle_type& handle, write_op_type op) {
			async_write(handle, op.get());
		}
	};

}

#endif // 

#endif
