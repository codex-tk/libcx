#include "tests/gprintf.hpp"
#include <cx/io/basic_engine.hpp>

class impl {
public:
	impl(void) { handle = 32; }
	int handle;
};

class foo_service {
public:
	foo_service(cx::io::implementation& impl)
		: _impl(impl)
	{

	}
	cx::io::implementation& _impl;
};


TEST(basic_engine, t0) {
	cx::io::basic_engine< cx::io::implementation, foo_service > engine;
	ASSERT_EQ(engine.template service<foo_service>()._impl.run(std::chrono::milliseconds(1)), 0);
	int value = 0;
	engine.implementation().post_handler([&] {
		++value;
	});
	engine.implementation().post_handler([&] {
		++value;
	});
	ASSERT_EQ(engine.template service<foo_service>()._impl.run(std::chrono::milliseconds(1)), 2);
	ASSERT_EQ(value, 2);
}

TEST(basic_engine, socket_service) {
	cx::io::basic_engine<
		cx::io::implementation
		, cx::io::ip::tcp::service
		, cx::io::ip::udp::service > engine;
	ASSERT_EQ(engine.implementation().run(std::chrono::milliseconds(1)), 0);
	int value = 0;
	engine.template service<cx::io::ip::tcp::service>().implementation().post_handler([&] {
		++value;
	});
	engine.template service<cx::io::ip::udp::service>().implementation().post_handler([&] {
		++value;
	});
	ASSERT_EQ(engine.template service<cx::io::ip::udp::service>().implementation().run(std::chrono::milliseconds(1)), 2);
	ASSERT_EQ(value, 2);
}


#if defined(CX_PLATFORM_WIN32) 
TEST(union_test, to) {
	struct handle {
		union {
			SOCKET s;
			HANDLE h;
		} fd;
	};

	handle h;
	h.fd.s = INVALID_SOCKET;
}

TEST(basic_engine, tcp) {
	cx::io::basic_engine<
		cx::io::implementation
		, cx::io::ip::tcp::service
		, cx::io::ip::udp::service > engine;
	cx::io::ip::tcp::socket fd(engine);
	ASSERT_TRUE(fd.handle().get() != nullptr);
}

#endif
