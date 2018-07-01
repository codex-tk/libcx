#include "gtest/gtest.h"
#include "tests/gprintf.hpp"
#include <cx/io/basic_engine.hpp>
#include <cx/io/ip/basic_acceptor.hpp>

TEST(cx_io_ip_basic_acceptor, t0) {
	cx::io::engine<
		cx::io::ip::tcp::service
	> engine;

	cx::io::ip::tcp::acceptor acceptor(engine);

	ASSERT_TRUE(acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET)));

	cx::io::ip::tcp::socket client(engine);
	ASSERT_TRUE(client.open(cx::io::ip::tcp::address::any(0, AF_INET)));
	ASSERT_TRUE(client.connect(cx::io::ip::tcp::address("127.0.0.1", 7543, AF_INET)));

	cx::io::ip::tcp::address addr;
	auto accepted = acceptor.accept(addr);
	ASSERT_TRUE(accepted != cx::io::ip::tcp::service::invalid_native_handle);
	cx::io::ip::tcp::socket fd(engine, accepted);
	fd.close();
	client.close();
	acceptor.close();
}


TEST(cx_io_ip_basic_acceptor, sample_echo) {

	cx::io::engine<
		cx::io::ip::tcp::service
	> engine;

	cx::io::ip::tcp::acceptor acceptor(engine);
	ASSERT_TRUE(acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET)));
	ASSERT_TRUE(acceptor.set_option(cx::io::ip::option::reuse_address()));

	cx::io::ip::tcp::address addr("127.0.0.1", 7543, AF_INET);

	cx::io::ip::tcp::socket client(engine);
	ASSERT_TRUE(client.open(addr));
	ASSERT_TRUE(client.connect(addr, std::chrono::milliseconds(1000)));

	cx::io::ip::tcp::address aaddr;
	auto acfd = acceptor.accept(aaddr, std::chrono::milliseconds(1000));
	auto accepted = cx::io::ip::tcp::socket(engine,acfd);
	ASSERT_TRUE(accepted);

	char buf[1024] = { 0 , };
	cx::io::ip::tcp::buffer rdbuf(buf, 1024);

	ASSERT_TRUE(client.write(cx::io::ip::tcp::buffer("Hello"), std::chrono::milliseconds(1000))
		== static_cast<int>(strlen("Hello")));
	ASSERT_TRUE(accepted.read(rdbuf, std::chrono::milliseconds(1000))== static_cast<int>(strlen("Hello")));
	ASSERT_STREQ("Hello", buf);

	accepted.write(cx::io::ip::tcp::buffer("World"));
	ASSERT_TRUE(client.read(rdbuf, std::chrono::milliseconds(1000)) == static_cast<int>(strlen("World")));
	ASSERT_STREQ("World", buf);

	acceptor.close();
	accepted.close();
	client.close();
}

TEST(cx_io_ip_basic_acceptor, timeout ) {
	cx::io::engine<
		cx::io::ip::tcp::service
	> engine;

	cx::io::ip::tcp::acceptor acceptor(engine);

	ASSERT_TRUE(acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET)));

	cx::io::ip::tcp::address addr;
	auto ac = acceptor.accept(addr, std::chrono::milliseconds(100));
	auto accepted = cx::io::ip::tcp::socket(engine,ac);
	ASSERT_FALSE(accepted);

	acceptor.close();
}

TEST(cx_io_ip_basic_acceptor, async) {
	cx::io::engine<
		cx::io::ip::tcp::service
	> engine;

	cx::io::ip::tcp::acceptor acceptor(engine);

	ASSERT_TRUE(acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET)));

	cx::io::ip::tcp::socket client(engine);
	ASSERT_TRUE(client.open(cx::io::ip::tcp::address::any(0, AF_INET)));
	ASSERT_TRUE(client.connect(cx::io::ip::tcp::address("127.0.0.1", 7543, AF_INET)));

	bool accepted = false;

	acceptor.async_accept([&](const std::error_code& e
		, cx::io::ip::tcp::service::native_handle_type raw_handle
		, const cx::io::ip::tcp::address& /*addr*/)
	{
		if (!e) {
			accepted = true;
		} else {
			std::string msg = e.message();
		}
		cx::io::ip::tcp::socket afd(engine , raw_handle);
		afd.close();
		acceptor.close();
		client.close();
	});
	engine.run();
	ASSERT_TRUE(accepted);
}