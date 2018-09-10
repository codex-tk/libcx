/**
 * @brief 
 * 
 * @file basic_udp.cpp
 * @author ghtak
 * @date 2018-09-09
 */

#include <gtest/gtest.h>
#include <cx/io/ip/services.hpp>

using buffer_type = cx::io::ip::udp::socket::buffer_type;

class udp_server {
public:
	udp_server(cx::io::engine& e)
		: _fd(e), _buf(1024)
	{
	}
	bool open() {
		auto server_address = cx::io::ip::udp::address::any(7543, AF_INET);
		if (_fd.open(server_address)) {
			if (_fd.set_option(cx::io::ip::option::reuse_address())) {
				if (_fd.bind(server_address)) {
					return true;
				}
			}
		}
		return false;
	}

	void close() {
		_fd.close();
	}

	bool echo() {
		cx::io::ip::udp::address address;
		int read = _fd.recvfrom(buffer_type(_buf.wrptr(), _buf.wrsize()),
			address);
		if (read > 0) {
			_fd.sendto(buffer_type(_buf.rdptr(), read), address);
			return true;
		}
		return false;
	}
private:
	cx::io::ip::udp::socket _fd;
	cx::basic_buffer<uint8_t> _buf;
};



TEST(cx_io_socket, basic_udp) {
	cx::io::engine e;

	udp_server server(e);
	ASSERT_TRUE(server.open());

	cx::io::ip::udp::socket udp_client(e);

	cx::basic_buffer<uint8_t> client_buf(1024);
	cx::io::ip::udp::address client_addr("127.0.0.1", 7543, AF_INET);
	ASSERT_TRUE(udp_client.open(client_addr));
	client_buf << "Hello";
	ASSERT_EQ(udp_client.sendto(
		buffer_type(client_buf.rdptr(),client_buf.rdsize()),
		client_addr), 
		client_buf.rdsize());
	client_buf.clear();

	ASSERT_TRUE(server.echo());

	ASSERT_EQ(udp_client.poll(cx::io::pollin, std::chrono::milliseconds(1000)), cx::io::pollin);

	int recv_size = udp_client.recvfrom(
		buffer_type(client_buf.wrptr(), client_buf.wrsize()),
		client_addr);
	ASSERT_TRUE(recv_size > 0);
	client_buf.wrptr(recv_size);
	ASSERT_STREQ(reinterpret_cast<char*>(client_buf.rdptr()), "Hello" );
	server.close();
	udp_client.close();
}

extern void gprintf(const char* fmt, ...);

TEST(cx_io_socket, basic_async_udp) {
	cx::io::engine e;
	udp_server server(e);
	cx::io::ip::udp::socket udp_client(e);
	
	cx::basic_buffer<uint8_t> client_buf(1024);
	cx::io::ip::udp::address client_addr("127.0.0.1", 7543, AF_INET);

	std::error_code ecode;
	int iosize = 0;

	client_buf << "Hello";

	ASSERT_TRUE(server.open());
	ASSERT_TRUE(udp_client.open(client_addr));
	ASSERT_TRUE(e.multiplexer().bind(udp_client.descriptor()));

	udp_client.async_send(buffer_type(client_buf.rdptr(), client_buf.rdsize()),
		client_addr ,
		[&](const std::error_code& ec, int sz, const cx::io::ip::udp::address&  )
	{
		gprintf("Called");
		ecode = ec;
		iosize = sz;
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);

	ASSERT_FALSE(ecode);
	ASSERT_EQ(iosize, client_buf.rdsize());

	client_buf.clear();
	iosize = 0;

	ASSERT_TRUE(server.echo());

	auto buf = buffer_type(client_buf.wrptr(), client_buf.wrsize());
	udp_client.async_recv(
		buf,
		client_addr,
		[&](const std::error_code& ec, int sz, const cx::io::ip::udp::address& )
	{
		ecode = ec;
		client_buf.wrptr(sz);
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);

	ASSERT_FALSE(ecode);
	ASSERT_TRUE(client_buf.rdptr() > 0);

	ASSERT_STREQ(reinterpret_cast<char*>(client_buf.rdptr()), "Hello");
	server.close();
	udp_client.close();
}