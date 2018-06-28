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

#include <cx/io/internal/win32/iocp/implementation.hpp>
#include <cx/io/internal/win32/iocp/basic_socket_service_impl.hpp>
#include <cx/io/internal/win32/iocp/basic_socket_service_impl_tcp.hpp>

#include <cx/io/internal/linux/epoll.hpp>
#include <cx/io/internal/reactor/reactor_socket_service.hpp>

namespace cx::io {

#if CX_PLATFORM == CX_P_WINDOWS
	using implementation = cx::io::implementation;

	namespace ip::tcp {
		using service = cx::io::ip::basic_socket_service_impl<SOCK_STREAM, IPPROTO_TCP>;
    }
	namespace ip::udp {
		using service = cx::io::ip::basic_socket_service_impl<SOCK_DGRAM, IPPROTO_UDP>;
	}
	namespace ip::icmp {
		using service = cx::io::ip::basic_socket_service_impl<SOCK_RAW, IPPROTO_ICMP>;
	}

#elif CX_PLATFORM == CX_P_LINUX
	using implementation = cx::io::epoll;

    namespace ip::tcp {
		using service = cx::io::ip::reactor_socket_service<
            implementation , SOCK_STREAM, IPPROTO_TCP>;
    }
	namespace ip::udp {
		using service = cx::io::ip::reactor_socket_service<
            implementation , SOCK_DGRAM, IPPROTO_UDP>;
	}
	namespace ip::icmp {
		using service = cx::io::ip::reactor_socket_service<
			implementation, SOCK_RAW, IPPROTO_ICMP>;
	}
#elif CX_PLATFORM == CX_P_MACOSX
	using implementation = cx::io::kqueue;
#else
	using implementation = cx::io::poll;
#endif
	namespace ip::tcp {
		using address = typename service::address_type;
		using socket = cx::io::ip::basic_socket<service>;
		using buffer = typename service::buffer_type;
		using acceptor = cx::io::ip::basic_acceptor<service>;
		using accept_context = cx::io::ip::basic_accept_context<service>;
	}
	namespace ip::udp {
		using address = typename service::address_type;
		using socket = cx::io::ip::basic_socket<service>;
		using buffer = typename service::buffer_type;
	}
	namespace ip::icmp {
		using address = typename service::address_type;
		using socket = cx::io::ip::basic_socket<service>;
		using buffer = typename service::buffer_type;
	}
}

#include <cx/time/basic_timer.hpp>
#include <cx/time/internal/win32_timer_queue_service.hpp>
#include <cx/time/internal/reactor_timer_fd_service.hpp>

namespace cx::time {

#if CX_PLATFORM == CX_P_WINDOWS
	using timer_service = cx::time::win32_timer_queue_service< cx::io::implementation, 100 >;
#elif CX_PLATFORM == CX_P_LINUX
	using timer_service = cx::time::reactor_timer_fd_service< cx::io::implementation >;
#elif CX_PLATFORM == CX_P_MACOSX 

#else 

#endif
	using timer = cx::time::basic_timer< timer_service >;
}

#endif
