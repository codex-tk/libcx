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
#include <cx/io/ip/services/win32_overlapped_dgram_service.hpp>

namespace cx::testing {
#if defined(CX_PLATFORM_WIN32)
	using engine = cx::io::basic_engine<cx::io::mux::completion_port>;
	using service = cx::io::ip::win32_overlapped_dgram_service<engine>;
#elif defined(CX_PLATFORM_LINUX)
	using engine = cx::io::basic_engine<cx::io::mux::epoll>;
	using service = cx::io::ip::basic_dgram_service<engine>;
#endif
	
	using socket = cx::io::ip::basic_socket<engine, service>;
	using address = typename service::address_type;
}

namespace cx::io::ip {
	template <> struct is_dgram_available<cx::testing::service> : std::true_type {};
}

class udp_server {
public:
	udp_server(cx::testing::engine& e)
		: _fd(e), _buf(1024)
	{
	}
	bool open() {
		auto server_address = cx::testing::address::any(7543, AF_INET);
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
		cx::testing::address address;
		int read = _fd.recvfrom( 
			cx::testing::socket::buffer_type(_buf.wrptr(), _buf.wrsize()),
			address);
		if (read > 0) {
			_fd.sendto(cx::testing::socket::buffer_type(_buf.rdptr(), read), address);
			return true;
		}
		return false;
	}
private:
	cx::testing::socket _fd;
	cx::basic_buffer<uint8_t> _buf;
};

TEST(cx_io_socket, basic_udp) {
	cx::testing::engine e;

	udp_server server(e);
	ASSERT_TRUE(server.open());

	cx::testing::socket udp_client(e);

	cx::basic_buffer<uint8_t> client_buf(1024);
	cx::testing::address client_addr("127.0.0.1", 7543, AF_INET);
	ASSERT_TRUE(udp_client.open(client_addr));
	client_buf << "Hello";
	ASSERT_EQ(udp_client.sendto(
		cx::testing::socket::buffer_type(client_buf.rdptr(),client_buf.rdsize()), 
		client_addr), 
		client_buf.rdsize());
	client_buf.clear();

	ASSERT_TRUE(server.echo());

	ASSERT_EQ(udp_client.poll(cx::io::pollin, std::chrono::milliseconds(1000)), cx::io::pollin);

	int recv_size = udp_client.recvfrom(
		cx::testing::socket::buffer_type(client_buf.wrptr(), client_buf.wrsize()),
		client_addr);
	ASSERT_TRUE(recv_size > 0);
	client_buf.wrptr(recv_size);
	ASSERT_STREQ(reinterpret_cast<char*>(client_buf.rdptr()), "Hello" );
	server.close();
	udp_client.close();
}

extern void gprintf(const char* fmt, ...);

TEST(cx_io_socket, basic_async_udp) {
	cx::testing::engine e;

	udp_server server(e);
	ASSERT_TRUE(server.open());

	cx::testing::socket udp_client(e);

	cx::basic_buffer<uint8_t> client_buf(1024);
	cx::testing::address client_addr("127.0.0.1", 7543, AF_INET);
	ASSERT_TRUE(udp_client.open(client_addr));
	client_buf << "Hello";

	std::error_code ecode;
	int iosize = 0;

	e.multiplexer().bind(udp_client.descriptor());
	udp_client.async_send(cx::testing::socket::buffer_type(client_buf.rdptr(), client_buf.rdsize()),
		client_addr ,
		[&](const std::error_code& ec, int sz, const cx::testing::address&  )
	{
		gprintf("Called");
		ecode = ec;
		iosize = sz;
	});
	e.run(std::chrono::milliseconds(1000));

	ASSERT_FALSE(ecode);
	ASSERT_EQ(iosize, client_buf.rdsize());

	client_buf.clear();
	iosize = 0;

	ASSERT_TRUE(server.echo());

	auto buf = cx::testing::socket::buffer_type(client_buf.wrptr(), client_buf.wrsize());
	udp_client.async_recv(
		buf,
		client_addr,
		[&](const std::error_code& ec, int sz, const cx::testing::address& )
	{
		ecode = ec;
		client_buf.wrptr(sz);
	});
	
	e.run(std::chrono::milliseconds(1000));

	ASSERT_FALSE(ecode);
	ASSERT_TRUE(client_buf.rdptr() > 0);

	ASSERT_STREQ(reinterpret_cast<char*>(client_buf.rdptr()), "Hello");
	server.close();
	udp_client.close();
}