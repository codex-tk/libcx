/**
 */
#ifndef __cx_io_detail_completion_port_socket_h__
#define __cx_io_detail_completion_port_socket_h__

#include <cx/cxdefine.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/basic_buffer.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io {

namespace detail {
    class completion_port;
}

namespace ip::detail {

	template < int Type, int Proto >
	class basic_completion_port_socket_service {
	protected:
		~basic_completion_port_socket_service( void ) {}
	public:
		using implementation_type = cx::io::detail::completion_port;
		using handle_type = cx::io::detail::completion_port::handle_type;
		using address_type = cx::io::ip::basic_address< struct sockaddr_storage, Type, Proto >;

		bool open(handle_type handle, const address_type& address) {
			close(handle);
			handle->fd.s = ::WSASocketW(address.family()
				, address.type()
				, address.proto()
				, nullptr
				, 0
				, WSA_FLAG_OVERLAPPED);
			if (handle->fd.s != INVALID_SOCKET)
				return true;
			return false;
		}

		void close(handle_type handle) {
			::closesocket(handle->fd.s);
			handle->fd.s = INVALID_SOCKET;
		}

		bool connect(handle_type handle, const address_type& address) {
			if (::connect(handle->fd.s, address.sockaddr(), address.length()) == 0)
				return true;
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return true;
			//if ( errno == EINPROGRESS )
			//    return true;
			return false;
		}

		bool bind(handle_type handle, const address_type& address) {
			return ::bind(handle->fd.s, address.sockaddr(), address.length()) != SOCKET_ERROR;
		}

		int poll(handle_type handle
			, int ops
			, const std::chrono::milliseconds& ms)
		{
			WSAPOLLFD pollfd = { 0 };
			pollfd.fd = handle->fd.s;
			if (ops & cx::io::ops::read) {
				pollfd.events = POLLRDNORM;
			}
			if (ops & cx::io::ops::write) {
				pollfd.events |= POLLWRNORM;
			}
			if (SOCKET_ERROR == WSAPoll(&pollfd, 1, static_cast<int>(ms.count()))) {
				std::error_code ec(WSAGetLastError(), cx::windows_category());
				std::string msg = ec.message();
				return -1;
			}
			ops = 0;
			if (pollfd.revents & POLLRDNORM) {
				ops = cx::io::ops::read;
			}
			if (pollfd.revents & POLLWRNORM) {
				ops |= cx::io::ops::write;
			}
			return ops;
		}

		address_type local_address(handle_type handle) const {
			address_type addr;
			::getsockname(handle->fd.s, addr.sockaddr(), addr.length_ptr());
			return addr;
		}

		address_type remote_address(handle_type handle) const {
			address_type addr;
			::getpeername(handle->fd.s, addr.sockaddr(), addr.length_ptr());
			return addr;
		}

		template < typename T >
		bool set_option(handle_type handle, T&& opt) {
			return opt.set(handle->fd.s);
		}

		template < typename T >
		bool get_option(handle_type handle, T&& opt) {
			return opt.get(handle->fd.s);
		}
	private:
	};

    template < int Type , int Proto >
    class completion_port_socket_service ;
        
    template <> class completion_port_socket_service< SOCK_STREAM , IPPROTO_TCP >
		: public basic_completion_port_socket_service < SOCK_STREAM, IPPROTO_TCP >  {
    public:
		using buffer_type = cx::io::buffer;

		completion_port_socket_service(implementation_type& impl)
            : _implementation( impl )
        {}
        implementation_type& implementation( void ) {
            return _implementation;
        }

        int write( handle_type handle , const buffer_type& buf ){ 
            return send( handle->fd.s , static_cast<const char*>(buf.base()) , buf.length() , 0 );
        }

        int read( handle_type handle , buffer_type& buf ){ 
           return recv( handle->fd.s , static_cast<char*>(buf.base()) , buf.length() , 0 );
        }
    private:
        implementation_type& _implementation;
    };

    template <> class completion_port_socket_service< SOCK_DGRAM , IPPROTO_UDP>
		: public basic_completion_port_socket_service < SOCK_DGRAM, IPPROTO_UDP >  {
    public:
        struct _buffer_type {
            _buffer_type( void* ptr , std::size_t len ) 
                : buffer(ptr,len)
            {}
            _buffer_type( void ) 
                : buffer(nullptr,0)
            {
            }
            address_type address;
            cx::io::buffer buffer;
        };
        using buffer_type = _buffer_type;

        completion_port_socket_service(implementation_type& impl)
            : _implementation( impl )
        {}
        implementation_type& implementation( void ) {
            return _implementation;
        }

        int write( handle_type handle , const buffer_type& buf ){ 
            return sendto( handle->fd.s 
                , static_cast<const char*>(buf.buffer.base()) 
                , buf.buffer.length() 
                , 0 
                , buf.address.sockaddr()
                , buf.address.length());
        }

        int read( handle_type handle , buffer_type& buf ){ 
           return recvfrom( handle->fd.s 
                , static_cast<char*>(buf.buffer.base()) 
                , buf.buffer.length() 
                , 0 
                , buf.address.sockaddr()
                , buf.address.length_ptr());
        }
    private:
        implementation_type& _implementation;
    };

}}

#endif // 

#endif