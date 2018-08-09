/**
 */
#ifndef __cx_io_services_h__
#define __cx_io_services_h__

#include <cx/io/io.hpp>
#include <cx/io/basic_object.hpp>
#include <cx/io/basic_buffer.hpp>

#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/basic_acceptor.hpp>

#include <cx/io/internal/iocp/implementation.hpp>
#include <cx/io/internal/iocp/socket_service_tcp.hpp>

#include <cx/io/internal/epoll/implemenation.hpp>
#include <cx/io/internal/reactor/socket_service_tcp.hpp>

namespace cx::io {

#if defined(CX_PLATFORM_WIN32)
	using implementation = cx::io::internal::iocp::implementation;
	template < int type , int proto > using socket_service_impl
		= cx::io::internal::iocp::ip::socket_service< type , proto >;
#elif defined(CX_PLATFORM_LINUX)
	using implementation = cx::io::internal::epoll::implementation;
	template < int type , int proto > using socket_service_impl
		=  cx::io::internal::reactor::ip::socket_service< implementation , type , proto >;
#elif defined(CX_PLATFORM_MACOSX)

#else
	
#endif
	namespace ip::tcp {
		using service = socket_service_impl<SOCK_STREAM, IPPROTO_TCP>;
		using buffer = typename service::buffer_type;
		using address = typename service::address_type;
		using socket = cx::io::ip::basic_socket<service>;
		using acceptor = cx::io::ip::basic_acceptor<service>;
	}
	namespace ip::udp {
		using service = socket_service_impl<SOCK_DGRAM, IPPROTO_UDP>;
		using buffer = typename service::buffer_type;
		using address = typename service::address_type;
		using socket = cx::io::ip::basic_socket<service>;
	}
	namespace ip::icmp {
		using service = socket_service_impl<SOCK_RAW, IPPROTO_ICMP>;
		using buffer = typename service::buffer_type;
		using address = typename service::address_type;
		using socket = cx::io::ip::basic_socket<service>;
	}
}

#include <cx/time/basic_timer.hpp>
#include <cx/time/internal/win32_timer_queue_service.hpp>
#include <cx/time/internal/reactor_timer_fd_service.hpp>

namespace cx::time {

#if defined(CX_PLATFORM_WIN32)
	using timer_service = cx::time::win32_timer_queue_service< cx::io::implementation, 100 >;
#elif defined(CX_PLATFORM_LINUX)
	using timer_service = cx::time::reactor_timer_fd_service< cx::io::implementation >;
#elif defined(CX_PLATFORM_MACOSX)

#else 

#endif
	using timer = cx::time::basic_timer< timer_service >;
}

#endif
