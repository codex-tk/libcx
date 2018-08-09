/**
 */
#ifndef __cx_io_detail_completion_port_socket_h__
#define __cx_io_detail_completion_port_socket_h__

#if defined(CX_PLATFORM_WIN32)

#include <cx/io/internal/iocp/socket_service.hpp>

#include <cx/io/internal/iocp/ops/connect_op.hpp>
#include <cx/io/internal/iocp/ops/accept_op.hpp>

namespace cx::io::internal::iocp::ip {

	template <> class socket_service<SOCK_STREAM,IPPROTO_TCP>
		: public basic_socket_service<socket_service<SOCK_STREAM,IPPROTO_TCP>> {
	public:
		using this_type = socket_service;
		using buffer_type = cx::io::buffer;
		using basic_socket_service < this_type >::connect;
		
		template < typename HandlerType > using connect_op 
			= cx::io::internal::iocp::ip::connect_op< this_type, HandlerType>;
		template < typename HandlerType > using accept_op
			= cx::io::internal::iocp::ip::accept_op< this_type, HandlerType>;

		template < typename HandlerType > using read_op 
			= cx::io::handler_op< basic_read_op<this_type> , HandlerType>;
		template < typename HandlerType > using write_op 
			= cx::io::handler_op< basic_write_op<this_type> , HandlerType>;

		socket_service(implementation_type& impl)
			: basic_socket_service(impl) {}

		handle_type make_shared_handle(void) {
			return std::make_shared<_handle>(*this);
		}

		handle_type make_shared_handle(native_handle_type raw_handle) {
			handle_type handle = std::make_shared<_handle>(*this , raw_handle);
			implementation().bind(handle, 0);
			return handle;
		}

		int write(const handle_type& handle, const buffer_type& buf) {
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				last_error(std::make_error_code(std::errc::invalid_argument));
				return -1;
			}
			int ret = send(handle->fd.s, static_cast<const char*>(buf.base()), buf.length(), 0);
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
			int ret = recv(handle->fd.s, static_cast<char*>(buf.base()), buf.length(), 0);
			if (ret == SOCKET_ERROR) {
				last_error(cx::system_error());
				return -1;
			}
			return ret;
		}

		bool listen(const handle_type& handle, int backlog) {
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}
			if (::listen(handle->fd.s, backlog) == SOCKET_ERROR) {
				last_error(cx::system_error());
				return false;
			}
			return true;
		}

		native_handle_type accept(const handle_type& handle, address_type& addr) {
			return ::accept(handle->fd.s, addr.sockaddr(), addr.length_ptr());
		}

		template < typename HandlerType >
		void async_connect(const handle_type& handle, const address_type& addr, HandlerType&& handler) {
			connect_op<HandlerType>* op =
				new connect_op<HandlerType>(addr, std::forward<HandlerType>(handler));

			if (handle.get() == nullptr) {
				op->error(std::make_error_code(std::errc::invalid_argument));
				implementation().post(op);
				return;
			}

			if (handle->fd.s == invalid_native_handle) {
				if (!open(handle, addr)) {
					op->error(last_error());
					implementation().post(op);
					return;
				}
			}
			implementation().add_active_links();
			LPFN_CONNECTEX _connect_ex = nullptr;
			DWORD bytes_returned = 0;
			GUID guid = WSAID_CONNECTEX;
			if (WSAIoctl(handle->fd.s
				, SIO_GET_EXTENSION_FUNCTION_POINTER
				, &guid, sizeof(guid)
				, &_connect_ex, sizeof(_connect_ex)
				, &bytes_returned, nullptr, nullptr) != SOCKET_ERROR ) 
			{
				address_type bindaddr = address_type::any(0, addr.family());
				if (bind(handle, bindaddr)) {
					bytes_returned = 0;
					if (_connect_ex(handle->fd.s
						, op->address().sockaddr()
						, op->address().length()
						, nullptr, 0
						, &bytes_returned, op->overlapped()) == TRUE)
						return;
					if (WSAGetLastError() == WSA_IO_PENDING)
						return;
				}
			}
			op->error(cx::system_error());
			implementation().post(op);
			implementation().release_active_links();
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
			op->reset();
			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSASend(handle->fd.s
				, op->buffer().raw_buffer()
				, 1
				, &bytes_transferred
				, flag
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
			op->reset();
			DWORD flag = 0;
			DWORD bytes_transferred = 0;
			if (WSARecv(handle->fd.s
				, op->buffer().raw_buffer()
				, 1
				, &bytes_transferred
				, &flag
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
		void async_accept(const handle_type& handle, HandlerType&& handler) {
			accept_op<HandlerType>* op =
				new accept_op<HandlerType>(std::forward<HandlerType>(handler));
			
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				op->error(std::make_error_code(std::errc::invalid_argument));
				implementation().post(op);
				return;
			}

			implementation().add_active_links();

			address_type addr = this->local_address(handle);

			native_handle_type fd = ::WSASocketW(addr.family()
				, addr.type()
				, addr.proto()
				, nullptr
				, 0
				, WSA_FLAG_OVERLAPPED);

			if (fd != invalid_native_handle ) {
				op->raw_handle(fd);
				DWORD bytes_returned = 0;
				if (AcceptEx(handle->fd.s
					, fd
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
			op->error(cx::system_error());
			implementation().post(op);
			implementation().release_active_links();
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
