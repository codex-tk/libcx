/**
* @brief
*
* @file services.hpp
* @author ghtak
* @date 2018-09-11
*/

#include <cx/io/basic_engine.hpp>
#include <cx/io/ip/ip.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/services/basic_dgram_service.hpp>
#include <cx/io/ip/services/basic_stream_service.hpp>
#include <cx/io/ip/services/win32_overlapped_dgram_service.hpp>
#include <cx/io/ip/services/win32_overlapped_stream_service.hpp>

#include <cx/io/ip/basic_acceptor.hpp>

#include <cx/timer/basic_timer_service.hpp>
#include <cx/timer/basic_timer.hpp>

namespace cx::io {
#if defined(CX_PLATFORM_WIN32)
	using engine = cx::io::basic_engine<cx::io::mux::completion_port>;
#elif defined(CX_PLATFORM_LINUX)
	using engine = cx::io::basic_engine<cx::io::mux::epoll>;
#endif
}

namespace cx::io::ip {
	namespace udp {
#if defined(CX_PLATFORM_WIN32)
		using service = cx::io::ip::win32_overlapped_dgram_service<engine>;

#elif defined(CX_PLATFORM_LINUX)
		using service = cx::io::ip::basic_dgram_service<engine>;
#endif
	}
	template <> struct is_dgram_available<cx::io::ip::udp::service> : std::true_type {};

	namespace udp {
		using socket = cx::io::ip::basic_socket<cx::io::engine, cx::io::ip::udp::service>;
		using address = typename service::address_type;
	}
}

namespace cx::io::ip {
	namespace tcp {
#if defined(CX_PLATFORM_WIN32)
		using service = cx::io::ip::win32_overlapped_stream_service<engine>;
#elif defined(CX_PLATFORM_LINUX)
		using service = cx::io::ip::basic_stream_service<engine>;
#endif
	}

	template <> struct is_stream_available<cx::io::ip::tcp::service> : std::true_type {};

	namespace tcp {
		using socket = cx::io::ip::basic_socket<cx::io::engine, cx::io::ip::tcp::service>;
		using address = typename service::address_type;
		using acceptor = cx::io::ip::basic_acceptor<cx::io::engine, cx::io::ip::tcp::service>;
	}
}

namespace cx::timer {
	using timer = basic_timer<cx::io::engine>;
}