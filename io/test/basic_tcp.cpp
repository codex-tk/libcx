/**
 * @brief 
 * 
 * @file basic_tcp.cpp
 * @author ghtak
 * @date 2018-09-11
 */

#include <gtest/gtest.h>
#include <cx/io/services.hpp>
#include "gprintf.hpp"

using buffer_type = cx::io::ip::tcp::socket::buffer_type;
static const std::string get("GET / HTTP/1.1\r\n\r\n");

TEST(cx_io_socket, basic_tcp) {
	cx::io::engine e;
	cx::io::ip::tcp::socket fd(e);
	auto addresses = cx::io::ip::tcp::address::resolve("google.com", 80, AF_INET);
	ASSERT_FALSE(addresses.empty());
	ASSERT_TRUE(fd.open(addresses[0]));
	ASSERT_TRUE(fd.good());

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

	ASSERT_TRUE(fd.connect(addresses[0]));

	gprintf("%s", fd.remote_address().to_string().c_str());
	gprintf("%s", fd.local_address().to_string().c_str());

	buffer_type buf(get);
	fd.async_send(buf, [&](const std::error_code& ec, int sz) {
		ASSERT_FALSE(ec);
		ASSERT_TRUE(sz > 0);
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);

	char read_buf[1024] = { 0 , };
	buffer_type rdbuf(read_buf, 1024);

	fd.async_recv(
		rdbuf,
		[&](const std::error_code& ec, int sz)
	{
		ASSERT_FALSE(ec);
		ASSERT_TRUE(sz > 0);
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);
	fd.close();
}


TEST(cx_io_socket, basic_async_tcp_with_connect) {
	cx::io::engine e;
	cx::io::ip::tcp::socket fd(e);
	auto addresses = cx::io::ip::tcp::address::resolve("google.com", 80, AF_INET);
	ASSERT_FALSE(addresses.empty());
	ASSERT_TRUE(fd.open(addresses[0]));
	ASSERT_TRUE(fd.set_option(cx::io::ip::option::non_blocking()));
	ASSERT_TRUE(fd.good());
	fd.async_connect(addresses[0], [&](const std::error_code&, const cx::io::ip::tcp::address&) {

	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(10000)), 1);

	//gprintf("%s", fd.remote_address().to_string().c_str());
	//gprintf("%s", fd.local_address().to_string().c_str());

	buffer_type buf(get);
	fd.async_send(buf, [&](const std::error_code& ec, int sz) {
		ASSERT_FALSE(ec);
		ASSERT_TRUE(sz > 0);
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);

	char read_buf[1024] = { 0 , };
	buffer_type rdbuf(read_buf, 1024);

	fd.async_recv(
		rdbuf,
		[&](const std::error_code& ec, int sz)
	{
		ASSERT_FALSE(ec);
		ASSERT_TRUE(sz > 0);
	});

	ASSERT_EQ(e.run(std::chrono::milliseconds(1000)), 1);
	fd.close();
}

TEST(cx_io_acceptor, basic_accept) {
	cx::io::engine e;
	cx::io::ip::tcp::acceptor acceptor(e);

	std::error_code ec;

	ASSERT_TRUE(acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET), ec));

	ASSERT_FALSE(acceptor.poll(std::chrono::milliseconds(100)));

	acceptor.close();
}

namespace pre_req_ios_test {
	const char* hello = "Hello";
	int len = 6;

	class session : public std::enable_shared_from_this<session> {
	public:
		session(cx::io::engine& e)
			: _fd(e)
			, _read_buffer(pre_req_ios_test::len) {}

		void do_read(void) {
			auto pthis = shared_from_this();
			cx::basic_buffer<char> rdbuf(pre_req_ios_test::len);
			auto rb = cx::io::buffer(rdbuf.prepare(pre_req_ios_test::len),
				pre_req_ios_test::len);
			_fd.async_recv(rb,
				[&, rdbuf, pthis](const std::error_code& ec, int size)
			{
				cx::basic_buffer<char> buf(rdbuf);
				if (ec || size <= 0) {
					pthis->socket().close();
					return;
				}
				buf.commit(size);
				auto wrbuf = cx::deepcopy(buf);
				auto wb = cx::io::buffer(wrbuf.rdptr(), wrbuf.rdsize());
				_fd.async_send(wb,
					[&, pthis, wrbuf](const std::error_code&, int) {
				});
			});
		}

		cx::io::ip::tcp::socket& socket(void) { return _fd; }
	private:
		cx::io::ip::tcp::socket _fd;
		cx::basic_buffer<char> _read_buffer;
	};

	void async_accept(cx::io::engine& e, cx::io::ip::tcp::acceptor& acceptor) {
		std::shared_ptr<session> ptr(std::make_shared<session>(e));
		acceptor.async_accept(ptr->socket(),
			[&, ptr](const std::error_code&, cx::io::ip::tcp::address&)
		{
			ptr->do_read();
			ptr->do_read();
			acceptor.close();
		});
	}

}
TEST(cx_io_socket, pre_req_ios) {
	cx::io::engine e;
	cx::io::ip::tcp::acceptor acceptor(e);

	std::error_code ec;
	ASSERT_TRUE(acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET), ec));

	pre_req_ios_test::async_accept(e, acceptor);

	cx::io::ip::tcp::address target("127.0.0.1", 7543, AF_INET);
	cx::io::ip::tcp::socket client(e);
	ASSERT_TRUE(client.open(target));
	ASSERT_TRUE(client.set_option(cx::io::ip::option::non_blocking()));
	ASSERT_TRUE(client.good());

	client.async_connect(target, [&](const std::error_code& ec, const cx::io::ip::tcp::address&) {
		ASSERT_FALSE(ec);
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			cx::basic_buffer<char> wrbuf(pre_req_ios_test::len);
			wrbuf << pre_req_ios_test::hello;
			auto wb = cx::io::buffer(wrbuf.rdptr(), wrbuf.rdsize());
			client.async_send(wb, [&, wrbuf](const std::error_code& ec, const int size) {
				cx::basic_buffer<char> buf(wrbuf);
				ASSERT_FALSE(ec);
				ASSERT_EQ(size, pre_req_ios_test::len);
				buf.consume(size);
				auto rb = cx::io::buffer(buf.prepare(pre_req_ios_test::len), pre_req_ios_test::len);
				client.async_recv(rb,
					[&, buf](const std::error_code& ec, int size)
				{
					ASSERT_FALSE(ec);
					ASSERT_EQ(size, pre_req_ios_test::len);
				});
			});
		}
		{
			cx::basic_buffer<char> wrbuf(pre_req_ios_test::len);
			wrbuf << pre_req_ios_test::hello;
			auto wb = cx::io::buffer(wrbuf.rdptr(), wrbuf.rdsize());
			client.async_send(wb, [&, wrbuf](const std::error_code& ec, const int size) {
				cx::basic_buffer<char> buf(wrbuf);
				ASSERT_FALSE(ec);
				ASSERT_EQ(size, pre_req_ios_test::len);
				buf.consume(size);
				auto rb = cx::io::buffer(buf.prepare(pre_req_ios_test::len), pre_req_ios_test::len);
				client.async_recv(rb,
					[&, buf](const std::error_code& ec, int size)
				{
					ASSERT_FALSE(ec);
					ASSERT_EQ(size, pre_req_ios_test::len);
					client.close();
				});
			});
		}
	});
	int val = 0;
	while (val < 10) {
		val += e.run(std::chrono::milliseconds(100));
	}
	ASSERT_EQ(val, 10);
}


namespace readn_test {
	
	class session : public std::enable_shared_from_this<session> {
	public:
		session(cx::io::engine& e)
			: _fd(e)
			, _read_buffer(32) {}

		void do_read(void) {
			auto pthis = shared_from_this();
			cx::basic_buffer<char> rdbuf(32);
			auto rb = cx::io::buffer(rdbuf.prepare(32), 32);
			_fd.async_recvn(rb,
				[&, rdbuf, pthis](const std::error_code& ec, int size)
			{
				cx::basic_buffer<char> buf(rdbuf);
				if (ec || size <= 0) {
					pthis->socket().close();
					return;
				}
				buf.commit(size);
				auto wrbuf = cx::deepcopy(buf);
				auto wb = cx::io::buffer(wrbuf.rdptr(), wrbuf.rdsize());
				_fd.async_send(wb,
					[&, pthis, wrbuf](const std::error_code&, int) {
					_fd.close();
				});
			});
		}

		cx::io::ip::tcp::socket& socket(void) { return _fd; }
	private:
		cx::io::ip::tcp::socket _fd;
		cx::basic_buffer<char> _read_buffer;
	};

	void async_accept(cx::io::engine& e, cx::io::ip::tcp::acceptor& acceptor) {
		std::shared_ptr<session> ptr(std::make_shared<session>(e));
		acceptor.async_accept(ptr->socket(),
			[&, ptr](const std::error_code&, cx::io::ip::tcp::address&)
		{
			ptr->do_read();
			acceptor.close();
		});
	}

}
TEST(cx_io_socket, readn_test) {
	cx::io::engine e;
	cx::io::ip::tcp::acceptor acceptor(e);

	std::error_code ec;
	ASSERT_TRUE(acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET), ec));

	readn_test::async_accept(e, acceptor);

	cx::io::ip::tcp::address target("127.0.0.1", 7543, AF_INET);
	cx::io::ip::tcp::socket client(e);
	ASSERT_TRUE(client.open(target));
	ASSERT_TRUE(client.set_option(cx::io::ip::option::non_blocking()));
	ASSERT_TRUE(client.good());

	client.async_connect(target, [&](const std::error_code& ec, const cx::io::ip::tcp::address&) {
		gprintf("Client Connect");
		ASSERT_FALSE(ec);
	});
	int val = 0;
	while (val < 2) {
		val += e.run(std::chrono::milliseconds(100));
	}
	ASSERT_EQ(val, 2);
	cx::basic_buffer<char> rdbuf(32);
	auto rb = cx::io::buffer(rdbuf.prepare(32), 32);
	client.async_recvn(rb, [&](const std::error_code& ec, int size) {
		gprintf("Cl recvn %d", size);
		ASSERT_FALSE(ec);
		ASSERT_EQ(size, 32);
	});

	cx::basic_buffer<char> wrbuf(32);
	wrbuf << "1234567890123456789012345678901";
	ASSERT_EQ(wrbuf.rdsize(), 32);
	for (int i = 0; i < 5; ++i) {
		auto wb = cx::io::buffer(wrbuf.rdptr(), rand() % 6); 
		gprintf("Cl write %d", wb.length());
		client.async_sendn(wb, [&](const std::error_code& ec, int size) {
			gprintf("Cl on write %d", size);
			ASSERT_FALSE(ec);
			wrbuf.rdptr(size);
		});
		val = 0;
		while (val < 1) {
			val += e.run(std::chrono::milliseconds(100));
		}
		ASSERT_EQ(val, 1);
	}
	auto wb = cx::io::buffer(wrbuf.rdptr(), wrbuf.rdsize());
	gprintf("Cl write %d", wb.length());
	client.async_send(wb, [&](const std::error_code& ec, int size) {
		gprintf("Cl on write %d", size);
		ASSERT_FALSE(ec);
		wrbuf.rdptr(size);
	});
	val = 0;
	while (val < 3) {
		val += e.run(std::chrono::milliseconds(100));
	}
	ASSERT_EQ(val, 3);
	ASSERT_EQ(wrbuf.rdsize(), 0);
	client.close();
}