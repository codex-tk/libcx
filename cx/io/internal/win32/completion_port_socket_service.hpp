/**
 */
#ifndef __cx_io_detail_completion_port_socket_h__
#define __cx_io_detail_completion_port_socket_h__

#if CX_PLATFORM == CX_P_WINDOWS

#include <cx/cxdefine.hpp>

#include <cx/io/basic_buffer.hpp>
#include <cx/io/basic_read_op.hpp>
#include <cx/io/basic_write_op.hpp>
#include <cx/io/handler_op.hpp>
#include <cx/io/internal/win32/completion_port.hpp>
#include <cx/io/internal/win32/completion_port_connect_op.hpp>
#include <cx/io/internal/win32/completion_port_accept_op.hpp>

#include <cx/io/ip/hdr.hpp>
#include <cx/io/ip/basic_address.hpp>

namespace cx::io::ip {

	template < typename ServiceType >
	class basic_completion_port_socket_service;

	template < template < int , int > class ServiceType , int Type , int Proto >
	class basic_completion_port_socket_service< ServiceType<Type,Proto> >{
	protected:
		~basic_completion_port_socket_service(void) {}
	public:
		using implementation_type = cx::io::completion_port;
		using address_type = cx::io::ip::basic_address< struct sockaddr_storage, Type, Proto >;
		using operation_type = typename implementation_type::operation_type;
		using service_type = ServiceType<Type, Proto>;

		using native_handle_type = SOCKET;
		static const native_handle_type invalid_native_handle = INVALID_SOCKET;

		struct _handle : public cx::io::completion_port::basic_handle {
			_handle(service_type& svc)
				: service(svc)
			{
				this->fd.s = invalid_native_handle;
			}
			service_type& service;
		};

		using handle_type = std::shared_ptr<_handle>;


		basic_completion_port_socket_service(implementation_type& impl)
			: _implementation(impl) {}

		bool open(const handle_type& handle, const address_type& address) {
			close(handle);
			handle->fd.s = ::WSASocketW(address.family()
				, address.type()
				, address.proto()
				, nullptr
				, 0
				, WSA_FLAG_OVERLAPPED);
			if (handle->fd.s == invalid_native_handle) {
				last_error(cx::system_error());
				return false;
			}
			implementation().bind(handle, 0);
			return true;
		}

		void close(const handle_type& handle) {
			if (handle && handle->fd.s != invalid_native_handle) {
				implementation().unbind(handle);
				::closesocket(handle->fd.s);
				handle->fd.s = invalid_native_handle;
			}
		}

		bool connect(const handle_type& handle, const address_type& address) {
			if (handle.get() == nullptr) {
				last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}
			if (handle->fd.s == invalid_native_handle) {
				if (!open(handle, address)) {
					return false;
				}
			}
			if (::connect(handle->fd.s, address.sockaddr(), address.length()) != SOCKET_ERROR) {
				return true;
			}
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				return true;
			}
			last_error(cx::system_error());
			return false;
		}

		bool bind(const handle_type& handle, const address_type& address) {
			if(::bind(handle->fd.s, address.sockaddr(), address.length()) == SOCKET_ERROR){
				last_error(cx::system_error());
				return false;
			}
			return true;
		}

		int poll(const handle_type& handle
			, int ops
			, const std::chrono::milliseconds& ms)
		{
			WSAPOLLFD pollfd = { 0 };
			pollfd.fd = handle->fd.s;
			pollfd.events = ((ops & cx::io::pollin) ? POLLRDNORM : 0)
				| ((ops & cx::io::pollout) ? POLLWRNORM : 0);
			int result = WSAPoll(&pollfd, 1, static_cast<int>(ms.count()));
			if (result == SOCKET_ERROR) {
				last_error(cx::system_error());
				return -1;
			}
			if (result == 0) {
				last_error(std::make_error_code(std::errc::timed_out));
				return 0;
			}
			ops = ((pollfd.revents & POLLRDNORM) ? cx::io::pollin : 0)
				| ((pollfd.revents & POLLWRNORM) ? cx::io::pollout : 0);
			return ops;
		}

		address_type local_address(const handle_type& handle) {
			this->last_error(std::error_code{});
			address_type addr;
			if (::getsockname(handle->fd.s, addr.sockaddr(), addr.length_ptr()) == SOCKET_ERROR) {
				last_error(cx::system_error());
			}
			return addr;
		}

		address_type remote_address(const handle_type& handle) {
			this->last_error(std::error_code{});
			address_type addr;
			if(::getpeername(handle->fd.s, addr.sockaddr(), addr.length_ptr()) == SOCKET_ERROR){
				last_error(cx::system_error());
			}
			return addr;
		}

		template < typename T >
		bool set_option(const handle_type& handle, T&& opt) {
			if(!opt.set(handle->fd.s)){
				last_error(cx::system_error());
				return false;
			}
			return true;
		}

		template < typename T >
		bool get_option(const handle_type& handle, T&& opt) {
			if(!opt.get(handle->fd.s)){
				last_error(cx::system_error());
				return false;
			}
			return true;
		}

		bool good(const handle_type& handle) {
			if (handle) {
				return handle->fd.s != invalid_native_handle;
			}
			return false;
		}

		implementation_type& implementation(void) {
			return _implementation;
		}

		std::error_code last_error( std::error_code&& ec ) {
            return _implementation.last_error(ec);
        }

        std::error_code last_error(void) {
            return _implementation.last_error();
        }
	private:
		implementation_type& _implementation;
	};

	template < int Type, int Proto >
	class completion_port_socket_service
		: public basic_completion_port_socket_service <
			completion_port_socket_service<Type,Proto>
		> {
	public:
		using this_type = completion_port_socket_service;
		using address_type = typename basic_completion_port_socket_service<this_type>::address_type;
		using handle_type = typename basic_completion_port_socket_service<this_type>::handle_type;
		using native_handle_type = typename basic_completion_port_socket_service<this_type>::native_handle_type;
		using implementation_type = typename basic_completion_port_socket_service<this_type>::implementation_type;

		struct _buffer {
			_buffer(void* ptr, std::size_t len)
				: buffer(ptr, len)
			{}
			_buffer(void)
				: buffer(nullptr, 0)
			{
			}
			address_type address;
			cx::io::buffer buffer;
		};
		
		
		using buffer_type = _buffer;
		using basic_completion_port_socket_service < this_type >::connect;

		template < typename HandlerType > using read_op
			= cx::io::handler_op< basic_read_op<this_type>, HandlerType>;
		template < typename HandlerType > using write_op
			= cx::io::handler_op< basic_write_op<this_type>, HandlerType>;

		using read_op_type = std::shared_ptr<basic_read_op<this_type>>;
		using write_op_type = std::shared_ptr<basic_write_op<this_type>>;

		handle_type make_shared_handle(void) {
			return std::make_shared<_handle>(*this);
		}

		handle_type make_shared_handle(native_handle_type handle) {
			auto h = make_shared_handle();
			h->fd.s = handle;
			implementation().bind(h, 0);
			return h;
		}

		completion_port_socket_service(implementation_type& impl)
			: basic_completion_port_socket_service(impl) {}

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

		int shutdown(const handle_type&, int) {
			return 0;
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
				}
			}
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
				}
			}
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

	template <> class completion_port_socket_service< SOCK_STREAM, IPPROTO_TCP >
		: public basic_completion_port_socket_service < 
			completion_port_socket_service< SOCK_STREAM, IPPROTO_TCP >
		> {
	public:
		using this_type = completion_port_socket_service;
		using buffer_type = cx::io::buffer;
		using basic_completion_port_socket_service < this_type >::connect;
		
		template < typename HandlerType > using connect_op 
			= cx::io::ip::completion_port_connect_op< this_type, HandlerType>;
		template < typename HandlerType > using accept_op
			= cx::io::ip::completion_port_accept_op< this_type, HandlerType>;

		template < typename HandlerType > using read_op 
			= cx::io::handler_op< basic_read_op<this_type> , HandlerType>;
		template < typename HandlerType > using write_op 
			= cx::io::handler_op< basic_write_op<this_type> , HandlerType>;

		handle_type make_shared_handle(void) {
			return std::make_shared<_handle>(*this);
		}

		handle_type make_shared_handle(native_handle_type handle) {
			auto h = make_shared_handle();
			h->fd.s = handle;
			implementation().bind(h, 0);
			return h;
		}

		completion_port_socket_service(implementation_type& impl)
			: basic_completion_port_socket_service(impl) {}

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

		bool shutdown(const handle_type& handle, int how) {
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}
			if (::shutdown(handle->fd.s, how) == SOCKET_ERROR) {
				last_error(cx::system_error());
				return false;
			}
			return true;
		}

		basic_accept_context<this_type> accept(const handle_type& handle, address_type& addr) {
			native_handle_type fd = ::accept(handle->fd.s, addr.sockaddr(), addr.length_ptr());
			return basic_accept_context<this_type>(*this, fd);
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
				}
			}
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
				}
			}
		}


		template < typename HandlerType >
		void async_accept(const handle_type& handle, HandlerType&& handler) {
			accept_op<HandlerType>* op =
				new accept_op<HandlerType>( *this , std::forward<HandlerType>(handler));
			
			if (handle.get() == nullptr || handle->fd.s == invalid_native_handle) {
				op->error(std::make_error_code(std::errc::invalid_argument));
				implementation().post(op);
				return;
			}

			address_type addr = this->local_address(handle);

			native_handle_type fd = ::WSASocketW(addr.family()
				, addr.type()
				, addr.proto()
				, nullptr
				, 0
				, WSA_FLAG_OVERLAPPED);

			if (fd != invalid_native_handle ) {
				op->accept_context().handle(fd);
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
