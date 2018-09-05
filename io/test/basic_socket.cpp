/**
 * @brief 
 * 
 * @file basic_socket.cpp
 * @author ghtak
 * @date 2018-08-27
 */

#include <gtest/gtest.h>
#include <cx/io/basic_engine.hpp>
#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/services/basic_dgram_service.hpp>
#include <cx/io/ip/services/win32_overlapped_dgram_service.hpp>

#if defined(CX_PLATFORM_WIN32)

TEST(win32, handle_types) {
	ASSERT_EQ(-1, INVALID_SOCKET);
	ASSERT_EQ(-1, (int)INVALID_HANDLE_VALUE);
}

#endif
namespace cx::testing{
#if defined(CX_PLATFORM_WIN32)
	using engine = cx::io::basic_engine<cx::io::mux::completion_port>;
#elif defined(CX_PLATFORM_LINUX)
	using engine = cx::io::basic_engine<cx::io::mux::epoll>;
#endif
	using socket = cx::io::ip::basic_socket<engine, cx::io::ip::basic_dgram_service >;
	using address = cx::io::ip::basic_address<SOCK_STREAM, IPPROTO_TCP>;
}

TEST(cx_io_socket, base) {
	cx::testing::engine e;
	cx::testing::socket fd(e);
	cx::testing::address addr("127.0.0.1", 7543, AF_INET);
	ASSERT_TRUE(fd.open(addr));
}