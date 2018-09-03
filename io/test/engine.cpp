/**
 * @brief 
 * 
 * @file engine.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <gtest/gtest.h>
#include <cx/io/basic_engine.hpp>

#if defined(CX_PLATFORM_WIN32)
using engine_type = cx::io::basic_engine<cx::io::mux::completion_port>;
#elif defined(CX_PLATFORM_LINUX)
using engine_type = cx::io::basic_engine<cx::io::mux::epoll>;
#endif

namespace cx::testing {

	template < typename HandlerT >
	class _op : public engine_type::mux::operation {
	public:
		_op(HandlerT&& h)
			: _handler(std::forward<HandlerT>(h)) {

		}
		virtual bool complete(const engine_type::mux::descriptor_ptr&) override {
			return true;
		}

		virtual void request(const descriptor_ptr&) override {
		}

		virtual void operator()(void) override {
			_handler();
			delete this;
		}
	private:
		HandlerT _handler;
	};

	template < typename HandlerT >
	static engine_type::mux::operation* make_op(HandlerT&& h) {
		return new _op<HandlerT>(std::forward<HandlerT>(h));
	}
}

TEST(cx_io_engine, basic) {
	engine_type engine;
	int test_value = 0;
	engine.post(cx::testing::make_op([&] {
		test_value = 1;
	}));
	engine.run(std::chrono::milliseconds(1));
	ASSERT_EQ(test_value, 1);
}

