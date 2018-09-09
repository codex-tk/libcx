/**
 * @brief 
 * 
 * @file basic_udp.cpp
 * @author ghtak
 * @date 2018-09-09
 */

#include <gtest/gtest.h>
#include <cx/io/basic_engine.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/services/basic_dgram_service.hpp>

namespace cx::testing {
#if defined(CX_PLATFORM_WIN32)
	using engine = cx::io::basic_engine<cx::io::mux::completion_port>;
#elif defined(CX_PLATFORM_LINUX)
	using engine = cx::io::basic_engine<cx::io::mux::epoll>;
#endif
	using service = cx::io::ip::basic_dgram_service<engine>;
	using socket = cx::io::ip::basic_socket<engine, service>;
	using address = typename service::address_type;
}

TEST(cx_io_socket, basic_udp) {
	cx::testing::engine e;
	cx::testing::socket udp_server(e);
	cx::testing::socket udp_client(e);

	cx::basic_buffer<uint8_t> server_buf(1024);
	cx::basic_buffer<uint8_t> client_buf(1024);

	auto server_address = cx::testing::address::any(7543, AF_INET);
	cx::testing::address client_addr("127.0.0.1", 7543, AF_INET);

	ASSERT_TRUE(udp_server.open(server_address));
	ASSERT_TRUE(udp_server.set_option(cx::io::ip::option::reuse_address()));
	ASSERT_TRUE(udp_server.bind(server_address));

	ASSERT_TRUE(udp_client.open(client_addr));
	client_buf << "Hello";
	ASSERT_EQ(udp_client.sendto(client_buf, client_addr), client_buf.rdsize());

	int recv_size = udp_server.recvfrom(server_buf, server_address);
	ASSERT_EQ(recv_size, client_buf.rdsize());
	server_buf.wrptr(recv_size);
	ASSERT_STREQ(reinterpret_cast<char*>(client_buf.rdptr()), reinterpret_cast<char*>(server_buf.rdptr()));
	server_buf.clear();
	server_buf << "World";
	ASSERT_EQ(udp_server.sendto(server_buf, server_address), server_buf.rdsize());

	client_buf.clear();
	recv_size = udp_client.recvfrom(client_buf, client_addr);
	ASSERT_EQ(recv_size, server_buf.rdsize());
	client_buf.wrptr(recv_size);
	ASSERT_STREQ(reinterpret_cast<char*>(client_buf.rdptr()), reinterpret_cast<char*>(server_buf.rdptr()));

	udp_server.close();
	udp_client.close();
}