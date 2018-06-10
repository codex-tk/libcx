#include "tests/gprintf.hpp"
#include <cx/io/basic_engine.hpp>
#if CX_PLATFORM == CX_P_WINDOWS

TEST(ts, engine ) {
    cx::io::engine< cx::time::timer_service > engine;
	cx::time::timer timer(engine);
	cx::time::timer timer2(engine);
	auto at = std::chrono::system_clock::now();
	timer.expired_at(at);
	timer2.expired_at(at);
	int testValue = 0;
	timer.handler([&](const std::error_code& ec) {
		testValue = 1;
	});
	timer2.handler([&](const std::error_code& ec) {
		testValue = 2;
	});
	timer.fire();
	timer2.fire();
	ASSERT_EQ(engine.implementation().run(std::chrono::seconds(10)), 2);
	ASSERT_EQ(testValue, 2);
}


TEST(ts, cancel) {
	cx::io::engine< cx::time::timer_service > engine;
	cx::time::timer timer(engine);
	cx::time::timer timer2(engine);
	auto at = std::chrono::system_clock::now();
	timer.expired_at(at);
	timer2.expired_at(at + std::chrono::seconds(2));
	int testValue = 0;
	timer.handler([&](const std::error_code& ec) {
		testValue = 1;
	});
	timer2.handler([&](const std::error_code& ec) {
		if (ec == std::errc::operation_canceled)
			testValue = 3;
		else
			testValue = 2;
	});
	timer.fire();
	timer2.fire();
	ASSERT_EQ(engine.implementation().run(std::chrono::seconds(10)), 1);
	ASSERT_EQ(testValue, 1);
	timer2.cancel();
	ASSERT_EQ(engine.implementation().run(std::chrono::seconds(10)), 1);
	ASSERT_EQ(testValue, 3);
}

namespace cxtest {
	struct handle {
		std::chrono::system_clock::time_point expired_at;
		std::function< void(const std::error_code&) > handler;
	};
	using handle_type = std::shared_ptr<handle>;

	struct handle_type_cmp {
		bool operator()(handle_type t, handle_type u) {
			return t->expired_at > u->expired_at;
		}
	};
}

TEST(ts, pq) {
	std::priority_queue< cxtest::handle_type
		, std::vector<cxtest::handle_type>
		, cxtest::handle_type_cmp > queue;
	auto a = std::make_shared< cxtest::handle >();
	auto b = std::make_shared< cxtest::handle >();
	a->expired_at = std::chrono::system_clock::now();
	b->expired_at = std::chrono::system_clock::now() + std::chrono::milliseconds(10);
	queue.push(a);
	queue.push(b);

	ASSERT_EQ(queue.top()->expired_at, a->expired_at);
}
#endif
