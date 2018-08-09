/**
 * @brief
 *
 * @file option.hpp
 * @author ghtak
 * @date 2018-05-26
 */
#ifndef __cx_io_ip_option_h__
#define __cx_io_ip_option_h__

#include <cx/io/ip/basic_socket.hpp>

namespace cx::io::ip::option {

#if defined(CX_PLATFORM_WIN32)
	using socket_type = SOCKET;
#else
	using socket_type = int;
#endif
	enum { disable = 0, enable = 1, };

	template < typename T, int level, int opt >
	class value_option {
	public:
		value_option(void) : _option(enable) {}

		explicit value_option(const T& t) : _option(t) {}

		bool set(socket_type fd) {
			return setsockopt(fd, level, opt,
				reinterpret_cast<char*>(&_option),
				sizeof(_option)) != -1;
		}

		bool get(socket_type fd) {
			int size = sizeof(_option);
			return getsockopt(fd, level, opt,
				reinterpret_cast<char*>(&_option),
				&size) != -1;
		}

		T& value(void) { return _option; }

		void value(const T& value) { _option = value; }
	private:
		T _option;
	};

	class blocking {
	public:
		blocking(void) : _option(0) {}
		~blocking(void) {}
		bool set(socket_type fd) {
#if defined(CX_PLATFORM_WIN32)
			return ioctlsocket(fd, FIONBIO, &_option) != -1;
#else
			int x = fcntl(fd, F_GETFL, 0);
			return fcntl(fd, F_SETFL, x & ~O_NONBLOCK) != -1;
#endif
		}
	private:
		unsigned long _option;
	};

	class non_blocking {
	public:
		non_blocking(void) : _option(1) {}
		~non_blocking(void) {}
		bool set(socket_type fd) {
#if defined(CX_PLATFORM_WIN32)
			return ioctlsocket(fd, FIONBIO, &_option) != -1;
#else
			int x = fcntl(fd, F_GETFL, 0);
			return fcntl(fd, F_SETFL, x | O_NONBLOCK) != -1;
#endif
		}
	private:
		unsigned long _option;
	};

	using reuse_address = value_option< int, SOL_SOCKET, SO_REUSEADDR >;
	using bind_ipv6only = value_option< int, IPPROTO_IPV6, IPV6_V6ONLY >;
	using tcp_no_delay = value_option< int, IPPROTO_TCP, TCP_NODELAY >;
	/*
	typedef value_option< int       , SOL_SOCKET	, SO_BROADCAST			> broad_cast;
	typedef value_option< int		, SOL_SOCKET	, SO_RCVBUF				> recv_buffer;
	typedef value_option< int		, SOL_SOCKET	, SO_RCVTIMEO			> recv_time;
	typedef value_option< int		, SOL_SOCKET	, SO_SNDBUF				> send_buffer;
	typedef value_option< int		, SOL_SOCKET	, SO_SNDTIMEO			> send_time;
	typedef value_option< int		, SOL_SOCKET	, SO_ERROR 			    > error;
	typedef value_option< struct linger	, SOL_SOCKET	, SO_LINGER				> linger;

	static linger linger_remove_time_wait;

	typedef value_option< uint8_t	, IPPROTO_IP	, IP_MULTICAST_LOOP		> multi_cast_loop;
	typedef value_option< uint8_t	, IPPROTO_IP	, IP_MULTICAST_TTL		> multi_cast_ttl;
	typedef value_option< in_addr	, IPPROTO_IP	, IP_MULTICAST_IF		> multi_cast_if;
	typedef value_option< ip_mreq	, IPPROTO_IP	, IP_ADD_MEMBERSHIP		> multi_cast_join;
	typedef value_option< ip_mreq	, IPPROTO_IP	, IP_DROP_MEMBERSHIP	> multi_cast_drop;


	typedef value_option< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_LOOP	> multi_cast_loop_ipv6;
	typedef value_option< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_HOPS	> multi_cast_ttl_ipv6;
	typedef value_option< in6_addr	, IPPROTO_IPV6	, IPV6_MULTICAST_IF		> multi_cast_if_ipv6;
	typedef value_option< ipv6_mreq	, IPPROTO_IPV6	, IPV6_ADD_MEMBERSHIP	> multi_cast_join_ipv6;
	typedef value_option< ipv6_mreq	, IPPROTO_IPV6	, IPV6_DROP_MEMBERSHIP	> multi_cast_drop_ipv6;


#if defined(_WIN32) || defined(_WIN64)
	typedef value_option< BOOL		, SOL_SOCKET	, SO_CONDITIONAL_ACCEPT		> conditional_accept;
	typedef value_option< int		, SOL_SOCKET	, SO_CONNECT_TIME			> connect_time;
	typedef value_option< SOCKET	, SOL_SOCKET	, SO_UPDATE_ACCEPT_CONTEXT	> update_accept_context;
#endif
	*/
}

#endif