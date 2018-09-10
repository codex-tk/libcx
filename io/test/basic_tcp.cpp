/**
 * @brief 
 * 
 * @file basic_tcp.cpp
 * @author ghtak
 * @date 2018-09-11
 */

#include <gtest/gtest.h>
#include <cx/io/ip/services.hpp>

using buffer_type = cx::io::ip::tcp::socket::buffer_type;
static const std::string get("GET / HTTP/1.1\r\n\r\n");

TEST(cx_io_socket, basic_tcp) {
	cx::io::engine e;
	cx::io::ip::tcp::socket fd(e);
	auto addresses = cx::io::ip::tcp::address::resolve("google.com", 80, AF_INET);
	ASSERT_FALSE(addresses.empty());
	ASSERT_TRUE(fd.open(addresses[0]));
	ASSERT_TRUE(fd.good());

	ASSERT_TRUE(e.multiplexer().bind(fd.descriptor()));

	ASSERT_TRUE(fd.connect(addresses[0]));

	buffer_type buf(get);
	ASSERT_EQ(fd.send(buf), static_cast<int>(buf.length()));

	char read_buf[1024] = { 0 , };
	buffer_type rdbuf(read_buf, 1024);
	ASSERT_TRUE(fd.recv(rdbuf) > 0);
	fd.close();
	ASSERT_TRUE(!fd.good());
}


TEST(cx_io_socket, basic_async_tcp) {
	cx::io::engine e;
	cx::io::ip::tcp::socket fd(e);
	auto addresses = cx::io::ip::tcp::address::resolve("google.com", 80, AF_INET);
	ASSERT_FALSE(addresses.empty());
	ASSERT_TRUE(fd.open(addresses[0]));
	ASSERT_TRUE(fd.good());

	ASSERT_TRUE(e.multiplexer().bind(fd.descriptor()));

	ASSERT_TRUE(fd.connect(addresses[0]));

	buffer_type buf(get);
	fd.async_send(buf, [&](const std::error_code& ec, int sz) {
		ASSERT_FALSE(ec);
		ASSERT_TRUE(sz > 0);
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);


	char read_buf[1024] = { 0 , };
	buffer_type rdbuf(read_buf, 1024);

	fd.async_recv(
		buf,
		[&](const std::error_code& ec, int sz)
	{
		ASSERT_FALSE(ec);
		ASSERT_TRUE(sz > 0);
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);
	fd.close();
}