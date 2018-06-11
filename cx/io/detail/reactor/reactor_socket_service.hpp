/**
 */
#ifndef __cx_io_detail_reactor_socket_h__
#define __cx_io_detail_reactor_socket_h__

#include <cx/cxdefine.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/basic_buffer.hpp>
#include <cx/io/detail/reactor/basic_reactor.hpp>
#include <cx/io/detail/reactor/reactor_connect_op.hpp>
#include <cx/io/detail/reactor/reactor_accept_op.hpp>
#include <cx/io/detail/reactor/reactor_read_op.hpp>
#include <cx/io/detail/reactor/reactor_write_op.hpp>

#if CX_PLATFORM != CX_P_WINDOWS

namespace cx::io::ip {

	template < typename ImplementationType, int Type, int Proto >
	class reactor_socket_service;

	template <typename ImplementationType>
	class reactor_socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >
		: public cx::io::basic_reactor_socket_service <
		ImplementationType,
		reactor_socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >
		> {
	public:
		using implementation_type = ImplementationType;
		using this_type = reactor_socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >;
		using buffer_type = cx::io::buffer;
		using base_type = cx::io::basic_reactor_socket_service < ImplementationType, this_type >;
		using base_type::connect;
		using base_type::make_shared_handle;
		using address_type = typename base_type::address_type;
		using handle_type = typename base_type::handle_type;
        using operation_type = typename basic_reactor::operation_type;

        /*
		template < typename HandlerType > using connect_op 
            = cx::io::ip::reactor_connect_op< this_type, HandlerType>;
		template < typename HandlerType > using accept_op 
            = cx::io::ip::reactor_accept_op< this_type, HandlerType>;
		template < typename HandlerType > using read_op 
            = cx::io::reactor_read_op< this_type, HandlerType>;
		template < typename HandlerType > using write_op 
            = cx::io::reactor_write_op< this_type, HandlerType>;
            */

		handle_type make_shared_handle(void) {
			return this->make_shared_handle(*this);
		}

		reactor_socket_service(implementation_type& impl)
			: base_type(impl)
		{}

		int write(handle_type handle, const buffer_type& buf) {
			return send(handle->fd, static_cast<const char*>(buf.base()), buf.length(), 0);
		}

		int read(handle_type handle, buffer_type& buf) {
			return recv(handle->fd, static_cast<char*>(buf.base()), buf.length(), 0);
		}

		bool listen(handle_type handle, int backlog) {
			return ::listen(handle->fd, backlog) != -1;
		}

		int shutdown(handle_type handle, int how) {
			return ::shutdown(handle->fd, how);
		}

		handle_type accept(handle_type handle, address_type& addr) {
			handle_type accepted_handle = make_shared_handle();
			accepted_handle->fd = ::accept(handle->fd, addr.sockaddr(), addr.length_ptr());
			return accepted_handle;
		}

		template < typename HandlerType >
		void async_connect(handle_type handle, const address_type& addr, HandlerType&& handler) {
		}

		template < typename HandlerType >
		void async_write(handle_type handle, const buffer_type& buf, HandlerType&& handler) {
		}


		template < typename HandlerType >
		void async_read(handle_type handle, const buffer_type& buf, HandlerType&& handler) {
		}

		template < typename HandlerType >
		void async_accept(handle_type handle, const address_type& addr, HandlerType&& handler) {
		}

		bool connect_complete(handle_type handle, cx::io::ip::basic_connect_op<this_type>* op) {
			return true;
		}

		bool accept_complete(handle_type handle, cx::io::ip::basic_accept_op<this_type>* op) {
			return true;
		}

		bool write_complete(handle_type handle, cx::io::basic_write_op<this_type>* op) {
			return true;
		}

		bool read_complete(handle_type handle, cx::io::basic_read_op<this_type>* op) {
			return true;
		}
	private:

	};

	template <typename ImplementationType>
	class reactor_socket_service< ImplementationType, SOCK_DGRAM, IPPROTO_UDP >
		: public cx::io::basic_reactor_socket_service <
		ImplementationType,
		reactor_socket_service< ImplementationType, SOCK_DGRAM, IPPROTO_UDP >
		>
	{
	public:
		using implementation_type = ImplementationType;
		using this_type = reactor_socket_service< ImplementationType, SOCK_DGRAM, IPPROTO_UDP>;
		using base_type = cx::io::basic_reactor_socket_service < ImplementationType, this_type >;
		using base_type::connect;
		using base_type::make_shared_handle;
		using address_type = typename base_type::address_type;
		using handle_type = typename base_type::handle_type;
        using operation_type = typename basic_reactor::operation_type;

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

        /*
		template < typename HandlerType > using read_op = cx::io::reactor_read_op< this_type, HandlerType>;
		template < typename HandlerType > using write_op = cx::io::reactor_write_op< this_type, HandlerType>;

        */
		handle_type make_shared_handle(void) {
			return this->make_shared_handle(*this);
		}

		reactor_socket_service(implementation_type& impl)
			: base_type(impl)
		{}

		int write(handle_type handle, const buffer_type& buf) {
			return sendto(handle->fd
				, static_cast<const char*>(buf.buffer.base())
				, buf.buffer.length()
				, 0
				, buf.address.sockaddr()
				, buf.address.length());
		}

		int read(handle_type handle, buffer_type& buf) {
			return recvfrom(handle->fd
				, static_cast<char*>(buf.buffer.base())
				, buf.buffer.length()
				, 0
				, buf.address.sockaddr()
				, buf.address.length_ptr());
		}

		int shutdown(handle_type, int) {
			return 0;
		}

		template < typename HandlerType >
		void async_write(handle_type handle, const buffer_type& buf, HandlerType&& handler) {
		}

		template < typename HandlerType >
		void async_read(handle_type handle, const buffer_type& buf, HandlerType&& handler) {
		}

        bool write_complete(handle_type handle, cx::io::basic_write_op<this_type>* op) {
			return true;
		}

		bool read_complete(handle_type handle, cx::io::basic_read_op<this_type>* op) {
			return true;
		}
	};

}

#endif // 

#endif
